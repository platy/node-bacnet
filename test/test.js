var r = require('../build/Release/binding.node');
//console.log(r.whois('127.0.0.1'));
r.initDevice();
console.log(r.whois('192.168.1.255'));

r.listen();
