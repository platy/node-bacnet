const r = require('../bacnet.js');

r.initClient(function(iam) {
  console.log("iam: ", iam);
});
r.ee.on('iam', console.log)
r.initDevice();

r.whois('127.0.0.1', 260001, 260003)
r.whois('127.0.0.1', 260001, 260003)
//console.log(r.whois('192.168.1.255'));

r.listen();

setTimeout(function () {}, 1000);
