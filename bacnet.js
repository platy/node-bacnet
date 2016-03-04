const addon = require('./build/Release/binding.node')
const bacnet = Object.create(addon)

bacnet.init = function init (config) {
  const EventEmitter = require('events').EventEmitter
  var flatConfig = config && config.datalink || {} // I've flattened the config as I had trouble getting nested properties in the c++
  flatConfig.device_instance_id = config.device_instance_id
  const bacnetAddon = addon.init(flatConfig)

  const bacnetInterface = new EventEmitter()

  const confirmedCallbacks = {}
  function addCallback(invokeId, callback) {
    if (callback && invokeId > 0) {
      if (typeof callback !== 'function') throw new TypeError('non-function passed as callback argument')
      confirmedCallbacks[invokeId] = callback
    }
    return invokeId
  }

  bacnetAddon.initClient(bacnetInterface)
  if (config && config.device) bacnetAddon.initDevice()
  bacnetAddon.listen()

  bacnetInterface.whois = bacnetAddon.whois
  bacnetInterface.readProperty = function (deviceInstance, objectType, objectInstance, property, arrayIndex, callback) {
    if (!objectType) throw new TypeError('Expected an object type, got : ' + objectType)
    const invokeId = bacnetAddon.readProperty(deviceInstance, bacnet.objectTypeToNumber(objectType), objectInstance, bacnet.propertyKeyToNumber(property), arrayIndex)
    if (invokeId === 0) throw new Error('Invoking BACnet read failed')
    return addCallback(invokeId, callback)
  }
  bacnetInterface.writeProperty = function (deviceInstance, objectType, objectInstance, property, arrayIndex, value, callback) {
    if (!objectType) throw new TypeError('Expected an object type, got : ' + objectType)
    const invokeId = bacnetAddon.writeProperty(deviceInstance, bacnet.objectTypeToNumber(objectType), objectInstance, bacnet.propertyKeyToNumber(property), arrayIndex, new bacnet.BacnetValue(value))
    if (invokeId === 0) throw new Error('Invoking BACnet read failed')
    return addCallback(invokeId, callback)
  }

  bacnetInterface.on('ack', function (invokeId, response) {
    const invocationCallback = confirmedCallbacks[invokeId]
    if (invocationCallback) {
      delete confirmedCallbacks[invokeId]
      invocationCallback(null, response)
    }
  })

  bacnetInterface.on('abort', function (invokeId, reason) {
    console.log('abort', invokeId)
    const invocationCallback = confirmedCallbacks[invokeId]
    if (invocationCallback) {
      delete confirmedCallbacks[invokeId]
      invocationCallback(new Error(reason))
    }
  })

  bacnetInterface.on('error-ack', function (invokeId, error) {
    console.log('error', invokeId, error)
    const invocationCallback = confirmedCallbacks[invokeId]
    if (invocationCallback) {
      delete confirmedCallbacks[invokeId]
      invocationCallback(error)
    }
  })

  return bacnetInterface
}

module.exports = bacnet
