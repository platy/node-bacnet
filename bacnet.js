const EventEmitter = require('events').EventEmitter
const bacnetAddon = require('./build/Release/binding.node')

const bacnetInterface = new EventEmitter()

bacnetAddon.initClient(bacnetInterface)
bacnetInterface.initDevice = bacnetAddon.initDevice
bacnetInterface.listen = bacnetAddon.listen
bacnetInterface.whois = bacnetAddon.whois

module.exports = bacnetInterface
