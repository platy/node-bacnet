var device

function initializeDevice (config) {
  device = require('../../bacnet.js').init(config)
  process.send(true)
  process.on('message', handleMessage)
  device.on('iam', function (iam) {
    process.send({type: 'iam', event: iam})
  })
}

function handleMessage (message) {
  if (!message) { // exits on falsey
    process.removeAllListeners('message')
  } else { // rpc
    device[message.method].apply(null, message.args)
  }
}

process.once('message', initializeDevice)
