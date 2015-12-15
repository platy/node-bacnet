require('should')
const os = require('os')

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

const source = {
  mac: {
    ip: ip,
    port: 0xBAC0
  },
  network: 0
}

// TODO configure the bacnet device with these values

describe('Whois / Iam', function() {
  it('can reply to its own whois and iam messages with default settings', function(done) {
    const r = require('../bacnet.js').init({
      iface: iface,
      ip_port: undefined,
      apdu_timeout: undefined,
      apdu_retries: undefined,
      invoke_id: undefined,
      bbmd_port: undefined,
      bbmd_ttl: undefined,
      bbmd_address: undefined
    });
    r.initDevice();

    r.once('iam', function(iam) {
      console.log('iam', iam)
      iam.deviceId.should.equal(deviceId)
      iam.vendorId.should.equal(vendorId)
      iam.segmentation.should.equal(segmentation)
      iam.src.should.deepEqual(source)
      done()
    });

    r.listen()

    r.whois('127.0.0.1', 260001, 260003)
  })

  // todo test that the device id range is used properly
})
