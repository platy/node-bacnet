require('should')
const os = require('os')
const tools = require('./tools')

const deviceId = 260001
const vendorId = 260
const segmentation = 0b11

function getSuitableInterfaceAndIP() {
  const ifaces = os.networkInterfaces()
  for(var ifaceName of Object.keys(ifaces)) {
    for(var address of ifaces[ifaceName]) {
      if (!address.internal &&    // It would be nice to use the loopback interface, but it doesn't support broadcast which is how iam's are sent
        address.family === 'IPv4')
        return [ifaceName, address.address]
    }
  }
}

const ifaceIp = getSuitableInterfaceAndIP()
const iface = ifaceIp[0]
const ip = ifaceIp[1]

var r

describe('Whois / Iam', function() {
  it('can reply to its own whois and iam messages with default settings', function(done) {
    const device = tools.deviceProcess({
      datalink: {
        iface: iface
      },
      device: true
    })
    device.once('up', function() { // device up
      device.whois('127.0.0.1', 260001, 260003)
    })
    device.once('iam', function(iam) {
      console.log('iam', iam)
      iam.deviceId.should.equal(deviceId)
      iam.vendorId.should.equal(vendorId)
      iam.segmentation.should.equal(segmentation)
      iam.src.should.deepEqual({
          mac: {
            ip: ip,
            port: 0xBAC0
          },
          network: 0
        }
      )
      device.exit()
    });
    device.once('exit', done)
  })
  it('can reply to its own whois and iam messages with default settings', function(done) {
    const device = tools.deviceProcess({
      datalink: {
        iface: iface,
        ip_port: 47809
      },
      device: true
    })
    device.once('up', function() { // device up
      device.whois('127.0.0.1:47809', 260001, 260003)
    })
    device.once('iam', function(iam) {
      console.log('iam', iam)
      iam.deviceId.should.equal(deviceId)
      iam.vendorId.should.equal(vendorId)
      iam.segmentation.should.equal(segmentation)
      iam.src.should.deepEqual({
          mac: {
            ip: ip,
            port: 0xBAC1
          },
          network: 0
        }
      )
      device.exit()
      done()
    });
    device.once('exit', done)
  })

  xit('cannot reply to anothers whois and iam messages on a different port as the iam reply is broadcast on the listening port', function(done) {
    const device = tools.deviceProcess({
      datalink: {
        iface: iface,
        ip_port: 0xBAC1
      },
      device: true
    })
    device.once('up', function() { // device up
      r.whois('127.0.0.1:47809', 260001, 260003)
    })
    r.once('iam', function(iam) {
      console.log('iam', iam)
      iam.deviceId.should.equal(deviceId)
      iam.vendorId.should.equal(vendorId)
      iam.segmentation.should.equal(segmentation)
      iam.src.should.deepEqual(source)
      device.exit()
      done()
    });
  })

  // todo test that the device id range is used properly
})
