'use strict'

const async = require('async')
const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    iface: process.env.BACNET_INTERFACE,
    ip_port: process.env.BACNET_PORT || 0xBAC0
  },
  device: false
})
// inclusive range generator
const range = function * (from, to) {
  while (from < to) {
    yield from++
  }
}

r.on('error', (err) => console.log('error in bacnet', err))
let timeout = setTimeout(function () {}, 5000)

const deviceProperties = ['object-identifier', 'object-name', 'description', 'system-status', 'vendor-name',
  'vendor-identifier', 'model-name', 'firmware-revision', 'application-software-version', 'location', 'local-time', 'local-date',
  'utc-offset', 'daylight-savings-status', 'protocol-version', 'protocol-revision',
  'protocol-services-supported', /* 'object-types-supported' */ 96, 'object-list', 'max-apdu-length-accepted',
  'segmentation-supported', 'apdu-timeout', 'number-of-apdu-retries', /* 'device-address-binding', */ 'database-revision',
  'max-info-frames', 'max-master' // , 'active-cov-subscriptions'
]

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
    }), (err, array) => {
      propertyValue.value = array
      callback(err, propertyValue)
    })
  })
}

r.on('iam', function (iam) {
  console.log('iam: ', iam)
  let deviceId = iam.deviceId
  console.log(async.mapSeries(deviceProperties, (propertyName, propertyRead) => {
    const rpCallback = (err, propertyValue) => {
      if (err) {
        propertyRead(null, 'FAILED')
      } else {
        propertyRead(null, propertyValue.value)
      }
    }
    if (propertyName === 'object-list') { // may be too long to read whole thing
      readArrayPropertySequentially.call(r, deviceId, 'device', deviceId, propertyName, rpCallback)
    } else {
      r.readProperty(deviceId, 'device', deviceId, propertyName, false, rpCallback)
    }
  }, (err, values) => {
    if (err) {
      console.log('error', err)
    } else {
      console.log(iam.src.mac.ip, ':', deviceProperties.reduce((result, key, index) => {
        if (values[ index ].length === 1) result[ key ] = values[ index ][ 0 ]
        else result[ key ] = values[ index ]
        return result
      }, {}))
    }
    clearTimeout(timeout)
  }))
})

r.whois(process.argv[2])
