/* global describe, it, before, after */

require('should')
const bacnet = require('../bacnet.js')
const tools = require('./tools')
const async = require('async')

function * range (from, to) {
  while (from <= to) {
    yield from++
  }
}

const iface = tools.getSuitableBroadcastInterface()

describe('Confirmed requests', function () {
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
  // this tests that invoke ids are reused - there are only 256 of them
  const soakSize = 1000
  describe('when soaked in ' + soakSize + ' sequential confirmed requests', function () {
    before('soak', function (done) {
      device.once('error', done)
      async.mapSeries(Array.from(range(1, soakSize)), (requestNo, requestComplete) => {
        device.once('ack', (invokeId) => {
          requestComplete()
        })
        device.readProperty('127.0.0.1', 'device', 260001, propertyKey, false)
      }, (err) => {
        device.removeAllListeners('read-property-ack')
        device.removeAllListeners('error')
        if (err) return done(err)
        done()
      })
    })
    const propertyKey = 'description'
    it('can read the \'' + propertyKey + '\' property', function (done) {
      device.once('error', done)
      device.once('read-property-ack', (property) => {
        property.object.should.deepEqual({ type: 'device', instance: 260001 })
        property.property.should.equal(bacnet.propertyKeyToNumber(propertyKey))
        property.value.should.deepEqual([ 'server' ])
        done()
      })
      device.readProperty('127.0.0.1', 'device', 260001, propertyKey, false)
    })
  })
})
