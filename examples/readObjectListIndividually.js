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

r.on('error', err => console.log('error in bacnet', err))
let timeout = setTimeout(function () {}, 5000)

// Reads an a property by first checking its length and then reading each element, calls the callback with the full array
// TODO: add to bacnet.js - probably remove the dependency on async
function readArrayPropertySequentially (deviceAddress, objectType, objectInstance, propertyKey, callback) {
  this.readProperty(deviceAddress, objectType, Number(objectInstance), propertyKey, 0, (err, propertyValue) => {
    if (err) {
      console.log('Failed to read object-list length for device', deviceAddress)
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

readArrayPropertySequentially.call(r, process.argv[2], 'device', process.argv[3], 'object-list', (err, objects) => {
  if (err) console.log('Error reading object list for ' + process.argv[2])
  else console.log('objects for device ' + process.argv[2], objects)
  clearTimeout(timeout)
})
