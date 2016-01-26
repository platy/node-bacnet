'use strict'

const async = require('async')
const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    iface: process.argv[2],
    ip_port: 0xBAC0
  },
  device: false
})
// inclusive range generator
const range = function * (from, to) {
  while (from < to) {
    yield from++
  }
}

r.on('error', err => console.log('error in bacnet', err))

const deviceProperties = ['object-identifier', 'object-name', 'description', 'system-status', 'vendor-name',
  'vendor-identifier', 'model-name', 'firmware-revision', 'application-software-version', 'location', 'local-time', 'local-date',
  'utc-offset', 'daylight-savings-status', 'protocol-version', 'protocol-revision',
  'protocol-services-supported', /* 'object-types-supported' */ 96, /* may be too long 'object-list', */ 'max-apdu-length-accepted',
  'segmentation-supported', 'apdu-timeout', 'number-of-apdu-retries', /* 'device-address-binding', */ 'database-revision',
  'max-info-frames', 'max-master' // , 'active-cov-subscriptions'
]

r.on('iam', function (iam) {
  console.log('iam: ', iam)
  let deviceId = iam.deviceId
  async.mapSeries(deviceProperties, (propertyName, propertyRead) => r.readProperty(deviceId, 'device', deviceId, propertyName, false, (err, propertyValue) => {
    if (err) {
      propertyRead(null, 'FAILED')
    } else {
      propertyRead(null, propertyValue.value)
    }
  }), (err, values) => {
    if (err) {
      return console.log('error', err)
    }
    console.log(deviceProperties.reduce((result, key, index) => {
      if (values[index].length === 1) result[key] = values[index][0]
      else result[key] = values[index]
      return result
    }, {}))
  })
  // read the object-list separately and not the whole array as it is often too big
  r.readProperty(deviceId, 'device', deviceId, 'object-list', /* array length */ 0, (err, propertyValue) => {
    if (err) {
      console.log('Failed to read object-list length for device', deviceId)
    } else {
      async.map(range(1, propertyValue.value), (index, indexRead) => r.readProperty(deviceId, 'device', deviceId, 'object-list', index, (err, propertyValue) => {
        if (err) {
          indexRead(new Error('Failed to read object-list[' + index + '] for device ' + deviceId))
        } else {
          indexRead(null, propertyValue.value)
        }
      }), (err, objects) => {
        if (err) return console.log('Error reading object list for ' + deviceId)
        console.log('objects for device ' + deviceId, objects)
      })
    }
  })
})

r.whois(process.argv[3])

setTimeout(function () {}, 1000)
