const b = require('./build/Release/binding.node').whois('127.0.0.1');

console.log(b.whois("192.168.1.255"));
