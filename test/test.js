const r = require('../bacnet.js').init({
  datalink: {
    iface: 'bridge100',
    ip_port: 0xBAC0
  },
  device: false
})

r.on('iam', function (iam) {
  console.log('iam: ', iam)
  r.readProperty(iam.deviceId, 8, iam.deviceId, 76)
})
r.on('read-property-ack', function (iam) {
  console.log('read property ack')
})

r.whois()

// r.whois('192.168.1.255')

setTimeout(function () {}, 1000)
