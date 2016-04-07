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
  function roundTripConversionTest (inputs, valueType) {
    inputs.forEach((value) => {
      it('works for [' + value + ']', () => {
        let bacnetValue = new BacnetValue(value, valueType)
        should(bacnetValue.valueOf()).equal(value)
        let bacnetBytes = bacnetValue.bytes()
        BacnetValue.fromBytes(bacnetBytes).should.deepEqual(bacnetValue)
      })
    })
  }
  describe('null conversion roundtrip', () => {
    roundTripConversionTest([null], 'Null')
  })
  describe('boolean conversion roundtrip', () => {
    roundTripConversionTest([true, false], 'Boolean')
  })
  describe('unsigned conversion roundtrip', () => {
    roundTripConversionTest([0, 4000000000], 'Unsigned Int')
  })
  describe('signed conversion roundtrip', () => {
    roundTripConversionTest([-2000000000, 0, 2000000000], 'Signed Int')
  })
  describe('float conversion roundtrip', () => {    // Float loses some precision during change between double (in js) and float
    roundTripConversionTest([0, 2 ^ -30, -2 ^ -30, 1e10, -1e10], 'Real')
  })
  describe('double conversion roundtrip', () => {
    roundTripConversionTest([0, 1e-37, -1e-37, 1e10, -1e10, Number.MAX_VALUE, Number.MIN_VALUE], 'Double')
  })
  describe('octet string conversion roundtrip', () => {
    roundTripConversionTest([new Buffer(0), new Buffer([0, 1, 2])], 'Octet String')
  })
  describe('character string conversion roundtrip', () => {
    roundTripConversionTest(['Hallå', 'världen'], 'Character String')
  })
  xdescribe('bit string conversion roundtrip', () => {
    roundTripConversionTest([new Buffer('hello')], 'Bit String')
  })
  describe('enumerated conversion roundtrip', () => {
    roundTripConversionTest([0, 200], 'Enumerated')
  })
  describe('date conversion roundtrip', () => {
    roundTripConversionTest([{year: 2016, month: 2, day: 16, weekday: 'Wednesday'}], 'Date')
  })
  describe('time conversion roundtrip', () => {
    roundTripConversionTest([{hour: 1, min: 2, sec: 3, hundredths: 4}], 'Time')
  })
  describe('object id conversion roundtrip', () => {
    roundTripConversionTest([{type: 'device', instance: 123456}], 'Object ID')
  })

  describe('type inference', () => {
    function typeInferenceTest (value, valueType) {
      it('infers type of [' + valueType + '] for value [' + value + ']', () => {
        let bacnetValue = new BacnetValue(value)
        should(bacnetValue.valueOf()).equal(value)
        let bacnetBytes = bacnetValue.bytes()
        BacnetValue.fromBytes(bacnetBytes).should.deepEqual(new BacnetValue(value, valueType))
      })
    }
    typeInferenceTest(null, 'Null')
    typeInferenceTest(true, 'Boolean')
    typeInferenceTest(0, 'Unsigned Int')
    typeInferenceTest(-1, 'Signed Int')
    typeInferenceTest(0.25, 'Real')
    typeInferenceTest('string', 'Character String')
    typeInferenceTest(new Buffer([0, 1]), 'Octet String')
    typeInferenceTest({year: 2016, month: 2, day: 16, weekday: 'Wednesday'}, 'Date')
    typeInferenceTest({hour: 1, min: 2, sec: 3, hundredths: 4}, 'Time')
    typeInferenceTest({type: 'device', instance: 123456}, 'Object ID')
    // TODO : infer types for date, time, object id are also possible
  })

  // round trip conversions wont make sure the endianness is correct
})
