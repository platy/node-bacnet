const bacnet = require('../bacnet.js')
const r = bacnet.init({
  datalink: {
    iface: 'bridge100',
    ip_port: 0xBAC0
  },
  device: false
})

console.log('writing property', bacnet.propertyKeyToString(process.argv[5]))

r.on('iam', function (iam) {
  console.log('iam: ', iam)
  r.writeProperty(Number(process.argv[2]), process.argv[3], process.argv[4], process.argv[5], false, JSON.parse(process.argv[6]), function (err) {
    if (err) console.log('error', err)
    else console.log('success')
  })
})

r.whois(Number(process.argv[2]))

setTimeout(function () {}, 1000)
