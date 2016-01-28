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
  r.readProperty(iam.deviceId, 8, iam.deviceId, 76)
})
r.on('read-property-ack', function (property) {
  console.log('Received property /', objectIdToString(property.object), '/', property.property)
  console.log(property.value.map(objectIdToString))
})

r.whois()

// r.whois('192.168.1.255')

setTimeout(function () {}, 1000)
