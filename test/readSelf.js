const bacnet = require('../bacnet.js')
const async = require('async')
const r = bacnet.init({
  datalink: {
    iface: 'bridge100',
    ip_port: 0xBAC0
  },
  device: true
})

function objectIdToString (objectId) {
  return bacnet.objectTypeToString(objectId.type) + '/' + objectId.instance
}

function receiveObjectList (err, property) {
  if (err) return console.log('ERROR', err)
  console.log('Received property /', objectIdToString(property.object), '/', bacnet.propertyKeyToString(property.property))
  async.mapSeries(property.value, function (objectId, objectRead) {
    async.mapSeries(['object-name', 'description', 'present-value'], (propertyName, propertyRead) =>
        r.readProperty('127.0.0.1', objectId.type, objectId.instance, propertyName, (err, propertyValue) => {
          if (err) {
            propertyRead(null, 'NONE')
          } else {
            propertyRead(null, propertyValue.value)
          }
        }),
      (err, values) => {
        if (err) {
          console.log('error', err)
          return objectRead(null, false)
        }
        objectRead(null, {
          id: objectIdToString(objectId),
          name: values[0],
          description: values[1],
          value: values[2]
        })
      })
  }, (err, result) => {
    if (err) {
      console.log('Error', err)
    }
    console.log('Result', result)
  })
}

r.readProperty('127.0.0.1', 'device', 260001, 'object-list', receiveObjectList)

setTimeout(function () {}, 1000)
