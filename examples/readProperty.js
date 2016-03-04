const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    iface: 'bridge100',
    ip_port: 0xBAC0
  },
  device: false
})

console.log('reading property', bacnet.propertyKeyToString(process.argv[5]))

function objectIdToString (objectId) {
  return bacnet.objectTypeToString(objectId.type) + '/' + objectId.instance
}

r.on('iam', function (iam) {
  console.log('iam: ', iam)
  r.readProperty(Number(process.argv[2]), process.argv[3], process.argv[4], process.argv[5], false, function (err, property) {
    console.log('Received property /', objectIdToString(property.object), '/', bacnet.propertyKeyToString(property.property))
    console.log(property.value)
  })
})

r.whois(Number(process.argv[2]))

setTimeout(function () {}, 1000)
