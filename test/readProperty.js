const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    iface: 'bridge100',
    ip_port: 0xBAC0
  },
  device: false
})

function objectIdToString (objectId) {
  return bacnet.objectTypeToString(objectId.type) + '/' + objectId.instance
}

r.on('iam', function (iam) {
  console.log('iam: ', iam)
  r.readProperty(process.argv[2], process.argv[3], process.argv[4], process.argv[5])
})
r.on('read-property-ack', function (property) {
  console.log('Received property /', objectIdToString(property.object), '/', bacnet.propertyKeyToString(property.property))
  console.log(property.value.map(objectIdToString))
})

r.whois()

// r.whois('192.168.1.255')

setTimeout(function () {}, 1000)
