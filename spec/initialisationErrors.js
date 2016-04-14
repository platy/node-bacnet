'use strict'
/* global describe, it, before, after */

require('should')
const tools = require('./tools')

const iface = tools.getSuitableBroadcastInterface()

describe('Initialisation', function () {
  var device, socket
  after('Exit the device fork', function (done) {
    if (device) {
      device.removeAllListeners()
      device.once('exit', done)
      device.exit()
    } else {
      done()
    }
  })
  after('Release the socket', function (done) {
    socket.close(done)
  })
  before(function (done) {
    socket = require('dgram').createSocket('udp4').bind(47808)
    socket.once('listening', done)
  })
  // disabled as currently the error doesn't work on linux - annoying
  it('fails if the port is already in use', (done) => {
    device = tools.deviceProcess({
      datalink: {
        iface: iface
      },
      device: true
    })
    device.once('up', (err) => {
      if (!err) {
        done(new Error('Second device reported as up on port which is already in use'))
      }
    })
    device.once('init-error', (err) => {
      err.should.startWith('Error: Failed to initialize data link')
      done()
    })
    device.once('exit', (exitCode) => done(new Error('Bacnet client terminated prematurely with exit code : ' + exitCode)))
  })
})
