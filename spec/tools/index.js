const fork = require('child_process').fork
const os = require('os')

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


exports.getSuitableInterface = function getSuitableInterface() {
  const ifaces = os.networkInterfaces()
  for(var ifaceName of Object.keys(ifaces)) {
    for(var address of ifaces[ifaceName]) {
      if (!address.internal &&    // It would be nice to use the loopback interface, but it doesn't support broadcast which is how iam's are sent
        address.family === 'IPv4')
        return ifaceName
    }
  }
}
exports.getInterfaceIP = function getInterfaceIP(ifaceName) {
  const ifaces = os.networkInterfaces()
  for(var address of ifaces[ifaceName]) {
    if (!address.internal &&
      address.family === 'IPv4')  // I think the bacnet library doesn't support ipv6 right now
      return address.address
  }
}
