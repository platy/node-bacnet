const util = require('util')
const EventEmitter = require('events');

var r = require('./build/Release/binding.node');
r.ee = new EventEmitter()

module.exports = r;
