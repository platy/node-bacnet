
exports.init = function init(config) {
  const EventEmitter = require('events').EventEmitter
  const bacnetAddon = require('./build/Release/binding.node').init(config && config.datalink)

  const bacnetInterface = new EventEmitter()

  bacnetAddon.initClient(bacnetInterface)
  if (config && config.device) bacnetAddon.initDevice()
  bacnetAddon.listen()

  bacnetInterface.whois = bacnetAddon.whois

  return bacnetInterface
}
