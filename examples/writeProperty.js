const bacnet = require('../bacnet.js')
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

var value = JSON.parse(process.argv[6])
if (process.argv.length > 6) {
  value = new bacnet.BacnetValue(value, process.argv[7])
}
console.log('writing property', bacnet.propertyKeyToString(process.argv[5]), 'value', value)

withAddressOrId(process.argv[2], function (addressOrId) {
  r.writeProperty(addressOrId, process.argv[3], process.argv[4], process.argv[5], false, value, function (err) {
    if (err) console.log('error', err)
    else console.log('success')
  })
})

setTimeout(function () {}, 1000)
