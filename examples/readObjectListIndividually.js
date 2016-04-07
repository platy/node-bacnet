'use strict'

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

// inclusive range generator
function * range (from, to) {
  while (from <= to) {
    yield from++
  }
}

r.on('error', (err) => console.log('error in bacnet', err))
let timeout = setTimeout(function () {}, 5000)

// Reads an a property by first checking its length and then reading each element, calls the callback with the full array
// TODO: add to bacnet.js - probably remove the dependency on async
function readArrayPropertySequentially (deviceAddress, objectType, objectInstance, propertyKey, callback) {
  this.readProperty(deviceAddress, objectType, Number(objectInstance), propertyKey, 0, (err, propertyValue) => {
    if (err) {
      return console.log('Failed to read object-list length for device', deviceAddress, err)
    }
    console.log('array length = ', propertyValue.value)
    async.mapSeries(Array.from(range(1, propertyValue.value)), (index, indexRead) => r.readProperty(deviceAddress, objectType, objectInstance, propertyKey, index, (err, propertyValue) => {
      if (err) {
        indexRead(new Error('Failed to read object-list[' + index + '] for device ' + deviceAddress))
      } else {
        indexRead(null, propertyValue.value)
      }
    }), callback)
  })
}

const deviceAddress = process.argv[2]
const deviceInstance = process.argv[3]

withAddressOrId(deviceAddress, function (deviceAddressOrId) {
  readArrayPropertySequentially.bind(r)(deviceAddressOrId, 'device', deviceInstance, 'object-list', (err, objects) => {
    if (err) return console.log('Error reading object list for ' + deviceAddressOrId)
    console.log('Read object list for ' + deviceAddressOrId)
    async.mapSeries(objects, (objectId, objectDone) => {
      r.readProperty(deviceAddressOrId, objectId.type, objectId.instance, 'object-name', false, (err, nameProperty) => {
        if (err) return objectDone(err)
        objectId.name = nameProperty.value[ 0 ]
        r.readProperty(deviceAddressOrId, objectId.type, objectId.instance, 'description', false, (err, descriptionProperty) => {
          if (err) return objectDone(null, objectId)
          objectId.description = descriptionProperty.value[ 0 ]
          objectDone(null, objectId)
        })
      })
    }, (err, objectsComplete) => {
      if (err) {
        console.log('Error reading object list extra info for ' + deviceAddressOrId)
        console.log('objects for device ' + deviceAddressOrId, objects.map((object) => object.type + '/' + object.instance))
      } else {
        console.log('objects for device ' + deviceAddressOrId, objectsComplete.map((object) => object.type + '/' + object.instance + ' : ' + object.name + ' - "' + object.description + '"'))
      }
      clearTimeout(timeout)
    })
  })
})
