const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    iface: process.env.BACNET_INTERFACE,
    ip_port: process.env.BACNET_PORT || 0xBAC0
  },
  device: false
})

function withAddressOrId (addressOrId, callback) {
  if (addressOrId.match(/^\d+$/)) { // integer - should be a device Id so we have to do a whois
    console.log('doing whois for ' + addressOrId)
    r.whois(Number(addressOrId))
    r.on('iam', function (iam) {
      console.log('iam: ', iam)
      callback(Number(addressOrId))
    })
  } else { // something else - should be an address so we dont need to do a whois
    callback(addressOrId)
  }
}

function objectIdToString (objectId) {
  return bacnet.objectTypeToString(objectId.type) + '/' + objectId.instance
}

console.log('reading property', bacnet.propertyKeyToString(process.argv[5]))

withAddressOrId(process.argv[2], function (addressOrId) {
  r.readProperty(addressOrId, process.argv[3], process.argv[4], process.argv[5], false, function (err, property) {
    if (err) throw console.log('Error', err)
    console.log('Received property /', objectIdToString(property.object), '/', bacnet.propertyKeyToString(property.property))
    console.log(property.value)
  })
})

setTimeout(function () {}, 1000)
