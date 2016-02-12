/* global describe, it, xit, before, afterEach */

require('should')
const tools = require('./tools')

const vendorId = 260
const segmentation = 0b11

const iface = tools.getSuitableBroadcastInterface()
const ip = tools.getInterfaceIP(iface)

describe('Whois / Iam', function () {
  var device
  afterEach('Exit the device fork', function (done) {
    device.once('exit', done)
    device.exit()
  })
  describe('with default settings', function () {
    before('Fork device', function (done) {
      device = tools.deviceProcess({
        datalink: {
          iface: iface
        },
        device: true
      })
      device.once('up', done)
    })
    it('can reply to its own ranged whois and iam messages', function (done) {
      device.once('iam', function (iam) {
        console.log('iam', iam)
        iam.deviceId.should.equal(260001)
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
        done()
      })
      device.whois('127.0.0.1', 260001, 260003)
    })
  })
  describe('on a different port', function () {
    before(function (done) {
      device = tools.deviceProcess({
        datalink: {
          iface: iface,
          ip_port: 47809
        },
        device: true
      })
      device.once('up', done)
    })
    it('can reply to its own broadcast whois and iam messages', function (done) {
      device.once('iam', function (iam) {
        console.log('iam', iam)
        iam.deviceId.should.equal(260001)
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
        done()
      })
      device.whois('127.0.0.1:47809')
    })
  })
  describe('with a different device id', function () {
    before(function (done) {
      device = tools.deviceProcess({
        device_instance_id: 10,
        datalink: {
          iface: iface
        },
        device: true
      })
      device.once('up', done)
    })
    it('can reply to its own specified whois and iam messages', function (done) {
      device.once('iam', function (iam) {
        console.log('iam', iam)
        iam.deviceId.should.equal(10)
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
        done()
      })
      device.whois('127.0.0.1', 10)
    })
  })

  xit('cannot reply to anothers whois and iam messages on a different port as the iam reply is broadcast on the listening port', function (done) {
    const device = tools.deviceProcess({
      datalink: {
        iface: iface,
        ip_port: 0xBAC1
      },
      device: true
    })
    device.once('up', function () { // device up
      device.whois('127.0.0.1:47809', 260001, 260003)
    })
    device.once('iam', function (iam) {
      console.log('iam', iam)
      iam.deviceId.should.equal(260001)
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
      done()
    })
  })

// todo test that the device id range is used properly
})
