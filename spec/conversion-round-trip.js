/* globals it, describe, xdescribe */
'use strict'

const should = require('should')
const BacnetValue = require('../bacnet.js').BacnetValue

describe('Application value converters between BACnet and js domain', () => {
  it('has a valid test as the equality validators are working', () => {
    BacnetValue(0, 4).should.deepEqual(BacnetValue(0, 4))
    BacnetValue(0, 4).should.not.deepEqual(BacnetValue(0, 5))
    BacnetValue(0, 4).should.not.deepEqual(BacnetValue(1, 4))
  })
  function roundTripConversionTest (inputs, roundTripTest) {
    inputs.forEach((input) => {
      it('works for [' + input + ']', () => {
        roundTripTest(input)
      })
    })
  }
  describe('null conversion roundtrip', () => {
    roundTripConversionTest([null], (value) => {
      let bacnetValue = new BacnetValue(value, 0)
      should(bacnetValue.valueOf()).equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('boolean conversion roundtrip', () => {
    roundTripConversionTest([true, false], (value) => {
      let bacnetValue = new BacnetValue(value, 1)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('unsigned conversion roundtrip', () => {
    roundTripConversionTest([0, 4000000000], (value) => {
      let bacnetValue = new BacnetValue(value, 2)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('signed conversion roundtrip', () => {
    roundTripConversionTest([-2000000000, 0, 2000000000], (value) => {
      let bacnetValue = new BacnetValue(value, 3)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('float conversion roundtrip', () => {    // Float loses some precision during change between double (in js) and float
    roundTripConversionTest([0, 2 ^ -30, -2 ^ -30, 1e10, -1e10], (value) => {
      let bacnetValue = new BacnetValue(value, 4)
      Number(bacnetValue).should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('double conversion roundtrip', () => {
    roundTripConversionTest([0, 1e-37, -1e-37, 1e10, -1e10, Number.MAX_VALUE, Number.MIN_VALUE], (value) => {
      let bacnetValue = new BacnetValue(value, 5)
      Number(bacnetValue).should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('octet string conversion roundtrip', () => {
    roundTripConversionTest([new Buffer(0), new Buffer([0, 1, 2])], (value) => {
      let bacnetValue = new BacnetValue(value, 6)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('character string conversion roundtrip', () => {
    roundTripConversionTest(['Hallå', 'världen'], (value) => {
      let bacnetValue = new BacnetValue(value, 7)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
      console.log(BacnetValue.fromBytes(bacnetBytes))
    })
  })
  xdescribe('bit string conversion roundtrip', () => {
    roundTripConversionTest([new Buffer('hello')], (value) => {
      let bacnetValue = new BacnetValue(value, 8)
      console.log(bacnetValue)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      console.log('produced bacnet bytes', bacnetBytes)
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
      console.log(bacnetValue)
    })
  })
  describe('enumerated conversion roundtrip', () => {
    roundTripConversionTest([0, 200], (value) => {
      let bacnetValue = new BacnetValue(value, 9)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('date conversion roundtrip', () => {
    roundTripConversionTest([{year: 2016, month: 2, day: 16, weekday: 'Wednesday'}], (value) => {
      let bacnetValue = new BacnetValue(value, 10)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('time conversion roundtrip', () => {
    roundTripConversionTest([{hour: 1, min: 2, sec: 3, hundredths: 4}], (value) => {
      let bacnetValue = new BacnetValue(value, 11)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })
  describe('object id conversion roundtrip', () => {
    roundTripConversionTest([{type: 'device', instance: 123456}], (value) => {
      let bacnetValue = new BacnetValue(value, 12)
      bacnetValue.valueOf().should.equal(value)
      let bacnetBytes = bacnetValue.bytes()
      BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
    })
  })

  // round trip conversions wont make sure the endianness is correct
})
