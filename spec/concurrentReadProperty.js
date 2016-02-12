/* global describe, it, before, after */

require('should')
const bacnet = require('../bacnet.js')
const tools = require('./tools')

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
  // this tests that we get the same number of acks as we make reads
  // TODO : actually test that the reads come back with the correct invokeIds
  // TODO : even without the above - just a few concurrent requests occasionally causes failures
  describe('reads concurrently', function () {
    const concurrentReads = 1
    var responseCount = 0
    const propertyKey = 'object-name'
    it('can read the \'' + propertyKey + '\' property concurrently ' + concurrentReads + ' times', function (done) {
      device.once('error', done)
      device.on('read-property-ack', (property) => {
        property.object.should.deepEqual({ type: 'device', instance: 260001 })
        property.property.should.equal(bacnet.propertyKeyToNumber(propertyKey))
        property.value.should.deepEqual([ 'SimpleServer' ])
        responseCount++
        if (responseCount === concurrentReads) done()
      })
      for (var i = 0; i < concurrentReads; i++) {
        device.readProperty('127.0.0.1', 'device', 260001, propertyKey, false)
      }
    })
  })
})
