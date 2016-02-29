/* global describe, it, before, after */

require('should')
const bacnet = require('../bacnet.js')
const tools = require('./tools')

const vendorId = 260

const iface = tools.getSuitableBroadcastInterface()

describe('SubscribeCOV', function () {
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
  describe('reading its own device object', function () {
    it('can subscribe to changes and be notified when the property is written', function (done) {
      device.subscribeCOV('127.0.0.1', 'device', 260001, 'description')
      device.once('read-property-ack', (property) => {
        property.object.should.deepEqual({type: 'device', instance: 260001})
        property.property.should.equal(bacnet.propertyKeyToNumber(propertyKey))
        property.value.should.deepEqual(value)
        done()
      })
      //device.readProperty('127.0.0.1', 'device', 260001, 'description', false)
    })
  })
})
