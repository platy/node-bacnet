const fork = require('child_process').fork

function runningDeviceMessage(message) {
  this.emit(message.type, message.event)
}

function exit() {
  this.send(false) // exits
}

function whois(mac, objectMin, objectMax) {
  this.send({method: 'whois', args: [mac, objectMin, objectMax]})
}

exports.deviceProcess = function deviceProcess(config) {
  const device = fork(__dirname + '/deviceFromString.js')
  device.send(config || false) // initialises with no args
  device.exit = exit
  device.whois = whois
  device.once('message', function() {
    device.emit('up')
    device.on('message', runningDeviceMessage)
  })
  return device
}
