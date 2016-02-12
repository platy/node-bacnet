/* global describe, it, before, after */

require('should')
const bacnet = require('../bacnet.js')
const tools = require('./tools')

const vendorId = 260

const iface = tools.getSuitableBroadcastInterface()

describe('Read property', function () {
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
  // this is the most basic way I can do repeatable tests for the read property as these are already exposed by its own
  // device object - the test is also useful to see if the exposed device object changes. In the future it would be
  // good to also test objects specifically set with values. But this test should remain to test that configuring these
  // values work - esp once these values can be configured
  describe('reading its own device object', function () {
    readOwnDeviceObjectPropertyTest('object-identifier', [{type: 'device', instance: 260001}])
    readOwnDeviceObjectPropertyTest('object-name', ['SimpleServer'])
    readOwnDeviceObjectPropertyTest('description', ['server'])
    readOwnDeviceObjectPropertyTest('system-status', ['operational'])
    readOwnDeviceObjectPropertyTest('vendor-name', ['BACnet Stack at SourceForge'])
    readOwnDeviceObjectPropertyTest('vendor-identifier', [vendorId])
    readOwnDeviceObjectPropertyTest('model-name', ['GNU'])
    readOwnDeviceObjectPropertyTest('firmware-revision', ['0.9.0'])
    readOwnDeviceObjectPropertyTest('application-software-version', ['1.0'])
    readOwnDeviceObjectPropertyTest('location', ['USA'])
    // readOwnDeviceObjectPropertyTest('local-time', [])
    // readOwnDeviceObjectPropertyTest('local-date', [])
    // readOwnDeviceObjectPropertyTest('utc-offset', [])
    // readOwnDeviceObjectPropertyTest('daylight-savings-status', [])
    readOwnDeviceObjectPropertyTest('protocol-version', [1])
    readOwnDeviceObjectPropertyTest('protocol-revision', [14])
    // readOwnDeviceObjectPropertyTest('protocol-services-supported', [])
    readOwnDeviceObjectPropertyTest('object-list', [{type: 'device', instance: 260001}])
    readOwnDeviceObjectPropertyTest('max-apdu-length-accepted', [1476])
    readOwnDeviceObjectPropertyTest('segmentation-supported', ['no-segmentation'])
    readOwnDeviceObjectPropertyTest('apdu-timeout', [3000])
    readOwnDeviceObjectPropertyTest('number-of-apdu-retries', [3])
    readOwnDeviceObjectPropertyTest('database-revision', [0])
    // readOwnDeviceObjectPropertyTest('max-info-frames', [])
    // readOwnDeviceObjectPropertyTest('max-master', [])
    function readOwnDeviceObjectPropertyTest (propertyKey, value) {
      it('can read the \'' + propertyKey + '\' property', function (done) {
        device.once('read-property-ack', (property) => {
          property.object.should.deepEqual({type: 'device', instance: 260001})
          property.property.should.equal(bacnet.propertyKeyToNumber(propertyKey))
          property.value.should.deepEqual(value)
          done()
        })
        device.readProperty('127.0.0.1', 'device', 260001, propertyKey, false)
      })
    }
  })
})
