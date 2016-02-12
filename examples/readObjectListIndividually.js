'use strict'

const bacnet = require('../bacnet.js')
const async = require('async')
const r = bacnet.init({
  datalink: {
    // iface: process.argv[2],
    ip_port: 0xBAC0
  },
  device: false
})
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
      return console.log('Failed to read object-list length for device', deviceAddress)
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
readArrayPropertySequentially.call(r, deviceAddress, 'device', deviceInstance, 'object-list', (err, objects) => {
  if (err) return console.log('Error reading object list for ' + deviceAddress)
  console.log('Read object list for ' + deviceAddress)
  async.mapSeries(objects, (objectId, objectDone) => {
    r.readProperty(deviceAddress, objectId.type, objectId.instance, 'object-name', false, (err, nameProperty) => {
      if (err) return objectDone(err)
      objectId.name = nameProperty.value[0]
      r.readProperty(deviceAddress, objectId.type, objectId.instance, 'description', false, (err, descriptionProperty) => {
        if (err) return objectDone(null, objectId)
        objectId.description = descriptionProperty.value[0]
        objectDone(null, objectId)
      })
    })
  }, (err, objectsComplete) => {
    if (err) {
      console.log('Error reading object list extra info for ' + deviceAddress)
      console.log('objects for device ' + deviceAddress, objects.map((object) => object.type + '/' + object.instance))
    } else {
      console.log('objects for device ' + deviceAddress, objectsComplete.map((object) => object.type + '/' + object.instance + ' : ' + object.name + ' - "' + object.description + '"'))
    }
    clearTimeout(timeout)
  })
})

// on the OB network :
// root@dev-open-berlin1:/node-bacnet-prototype# node examples/readObjectListIndividually.js 192.168.1.2 12
// Initialise client handlers
// array length =  null
// Read object list for 192.168.1.2
// objects for device 192.168.1.2 []
// root@dev-open-berlin1:/node-bacnet-prototype# node examples/readObjectListIndividually.js 192.168.1.1 11
// Initialise client handlers
// array length =  null
// Read object list for 192.168.1.1
// objects for device 192.168.1.1 []
