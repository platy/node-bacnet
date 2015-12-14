require('should')

const deviceId = 260001
const vendorId = 260
const segmentation = 0b11
const source = {
  mac: {
    ip: "127.0.0.1",
    port: 0xBAC0
  },
  network: 0
}

describe('Whois / Iam', function() {
  it('can reply to its own whois and iam messages', function(done) {
    const r = require('../bacnet.js');

    r.once('iam', function(iam) {
      console.log('iam', iam)
      iam.deviceId.should.equal(deviceId)
      iam.vendorId.should.equal(vendorId)
      iam.segmentation.should.equal(segmentation)
      iam.src.should.equal(source)
      done()
    });
    r.initDevice();

    r.listen();

    r.whois('127.0.0.1', 260001, 260003)
  })
})
