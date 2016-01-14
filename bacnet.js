exports.init = function init (config) {
  const EventEmitter = require('events').EventEmitter
  var flatConfig = config && config.datalink || {} // I've flattened the config as I had trouble getting nested properties in the c++
  flatConfig.device_instance_id = config.device_instance_id
  const bacnetAddon = require('./build/Release/binding.node').init(flatConfig)

  const bacnetInterface = new EventEmitter()

  bacnetAddon.initClient(bacnetInterface)
  if (config && config.device) bacnetAddon.initDevice()
  bacnetAddon.listen()

  bacnetInterface.whois = bacnetAddon.whois
  bacnetInterface.readProperty = bacnetAddon.readProperty

  return bacnetInterface
}
