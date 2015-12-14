const r = require('../bacnet.js');

r.on('iam', function(iam) {
  console.log("iam: ", iam);
});
r.initDevice();

r.listen();

r.whois('127.0.0.1', 260001, 260003)
r.whois('127.0.0.1', 260001, 260003)

//r.whois('192.168.1.255')

setTimeout(function () {}, 1000);
