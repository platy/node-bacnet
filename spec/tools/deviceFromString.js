var device

function initializeDevice (config) {
  process.on('message', handleMessage)
  try {
    device = require('../../bacnet.js').init(config)
    device.on('iam', function (iam) {
      process.send({type: 'iam', event: iam})
    })
    device.on('read-property-ack', function (event, invokeId) {
      process.send({type: 'read-property-ack', event: event})
    })
    device.on('write-property-ack', function (event, invokeId) {
      process.send({type: 'write-property-ack', event: event})
    })
    device.on('ack', function (invokeId, event) {
      process.send({type: 'ack', event: invokeId}) // TODO : test framework improvement to get the invoke id and event
    })
    device.on('error', function (event) {
      process.send({type: 'error', event: event})
    })
    device.on('error-ack', function (event) {
      process.send({type: 'error-ack', event: event})
    })
    device.on('abort', function (event) {
      process.send({type: 'abort', event: event})
    })
    device.on('reject', function (event) {
      process.send({type: 'reject', event: event})
    })
  } catch (err) {
    return process.send({type: 'init-error', event: err.stack})
  }
  process.send(false)
}

function handleMessage (message) {
  if (!message) { // exits on falsey
    if (device) {
      device.closeQueue()
    }
    process.removeAllListeners()
  } else { // rpc
    device[message.method].apply(null, message.args)
  }
}

process.once('message', initializeDevice)
