const bacnet = require('../bacnet.js')
const async = require('async')
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

function receiveObjectList (err, property) {
  if (err) return console.log('ERROR', err)
  console.log('Received property /', objectIdToString(property.object), '/', bacnet.propertyKeyToString(property.property))
  async.mapSeries(property.value, function (objectId, objectRead) {
    async.mapSeries(['object-name', 'description'], (propertyName, propertyRead) => r.readProperty(Number(process.argv[2]), objectId.type, objectId.instance, propertyName, false, (err, propertyValue) => {
      if (err) {
        propertyRead(null, 'NONE')
      } else {
        propertyRead(null, propertyValue.value)
      }
    }), (err, values) => {
      if (err) {
        console.log('error', err)
        return objectRead(null, false)
      }
      objectRead(null, {
        id: objectIdToString(objectId),
        name: values[0],
        description: values[1]
      })
    })
  }, (err, result) => {
    if (err) {
      console.log('Error', err)
    }
    console.log('Result', result)
  })
}

withAddressOrId(process.argv[2], function (addressOrId) {
  console.log('read-property invoke',
    r.readProperty(addressOrId, 'device', process.argv[2], 'object-list', false, receiveObjectList))
})

setTimeout(function () {}, 4000)
