/* global describe, it, before, after */

require('should')
const bacnet = require('../bacnet.js')
const tools = require('./tools')

const iface = tools.getSuitableBroadcastInterface()

describe('Write property', function () {
  var device
  after('Exit the device fork', function (done) {
    device.once('exit', done)
    device.exit()
  })
  before(function (done) {
    device = tools.deviceProcess({
      datalink: {
        iface: iface
      },
      device: true
    })
    device.once('up', done)
  })
  // my writes dont currently support arrays - only single values
  describe('writing its own device object', function () {
    writeAndReadOwnDeviceObjectPropertyTest('object-name', 'New device name')
    writeAndReadOwnDeviceObjectPropertyTest('apdu-timeout', 4000)
    function writeAndReadOwnDeviceObjectPropertyTest (propertyKey, value) {
      it('can write the \'' + propertyKey + '\' property', function (done) {
        device.once('read-property-ack', (property) => {
          property.object.should.deepEqual({type: 'device', instance: 260001})
          property.property.should.equal(bacnet.propertyKeyToNumber(propertyKey))
          property.value.should.deepEqual([value])
          done()
        })
        device.once('write-property-ack', () => {
          device.readProperty('127.0.0.1', 'device', 260001, propertyKey, false)
        })
        device.writeProperty('127.0.0.1', 'device', 260001, propertyKey, false, value)
        device.once('error', done)
      })
    }
  })
})
