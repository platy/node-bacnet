
exports.init = function init(config) {
  const EventEmitter = require('events').EventEmitter
  const bacnetAddon = require('./build/Release/binding.node').init(config)

  const bacnetInterface = new EventEmitter()

  bacnetAddon.initClient(bacnetInterface)
  bacnetInterface.initDevice = bacnetAddon.initDevice
  bacnetInterface.listen = bacnetAddon.listen
  bacnetInterface.whois = bacnetAddon.whois

  return bacnetInterface
}
