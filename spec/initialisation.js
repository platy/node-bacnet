require('should')
const fork = require('child_process').fork

describe('init', function() {
  it('works with no args', function(done) {
    fork('test/test.js').once('exit', function (exitCode, signal) {
      console.log(exitCode, signal)
      exitCode.should.equal(0)
      done()
    })
  })
})
