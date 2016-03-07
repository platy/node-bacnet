'use strict'
/* global describe, it, before, after */

require('should')
const tools = require('./tools')

const iface = tools.getSuitableBroadcastInterface()

describe('Initialisation', function () {
  var device, device2
  after('Exit the device1 fork', function (done) {
    device.once('exit', done)
    device.exit()
  })
  after('Exit the device2 fork', function (done) {
    device2.removeAllListeners()
    device2.once('exit', done)
    device2.exit()
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
  it('fails if the port is already in use', (done) => {
    device2 = tools.deviceProcess({
      datalink: {
        iface: iface
      },
      device: true
    })
    device2.once('up', () => done(new Error('Second device reported as up on port which is already in use')))
    device2.once('init-error', (err) => {
      err.should.equal('Error: Failed to initialize data link')
      done()
    })
    device2.once('exit', (exitCode) => done(new Error('Bacnet client terminated prematurely with exit code : ' + exitCode)))
  })
})
