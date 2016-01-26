'use strict'

const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    // iface: process.argv[2],
    ip_port: 0xBAC1
  },
  device: false
})

r.on('error', err => console.log('error in bacnet', err))

r.readProperty(process.argv[2], 'device', process.argv[3], 'object-list', 0, (err, propertyValue) => {
  if (err) {
    return console.log('error', err)
  }
  for (var i = 1; i <= propertyValue.value; i++) {
    r.readProperty(process.argv[2], 'device', process.argv[3], 'object-list', i, (err, propertyValue) => {
      if (err) {
        return console.log('error', err)
      }
      console.log(propertyValue.value)
    })
  }
})

setTimeout(function () {}, 1000)
