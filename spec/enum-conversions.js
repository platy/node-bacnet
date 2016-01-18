/* globals it, xit, describe */

const should = require('should')
const bacnet = require('../bacnet.js')

describe('bacnet enum conversions', () => {
  describe('objectTypeToString', () => {
    it('converts 8 to "device"', () => {
      bacnet.objectTypeToString(8).should.equal('device')
    })
    it('converts "device" to "device"', () => {
      bacnet.objectTypeToString('device').should.equal('device')
    })
    xit('errors on "foo"', () => {
      try {
        bacnet.objectTypeToString('foo')
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
    xit('errors on -1', () => {
      try {
        bacnet.objectTypeToString(-1)
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
  })
  describe('objectTypeToNumber', () => {
    it('converts 8 to 8', () => {
      bacnet.objectTypeToNumber(8).should.equal(8)
    })
    it('converts "device" to 8', () => {
      bacnet.objectTypeToNumber('device').should.equal(8)
    })
    xit('errors on "foo"', () => {
      try {
        bacnet.objectTypeToNumber('foo')
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
    xit('errors on -1', () => {
      try {
        bacnet.objectTypeToNumber(-1)
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
  })
  describe('propertyKeyToString', () => {
    it('converts 76 to "object-list"', () => {
      bacnet.propertyKeyToString(76).should.equal('object-list')
    })
    it('converts "object-list" to "object-list"', () => {
      bacnet.propertyKeyToString('object-list').should.equal('object-list')
    })
    xit('errors on "foo"', () => {
      try {
        bacnet.propertyKeyToString('foo')
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
    xit('errors on -1', () => {
      try {
        bacnet.propertyKeyToString(-1)
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
  })
  describe('propertyKeyToNumber', () => {
    it('converts 76 to 76', () => {
      bacnet.propertyKeyToNumber(76).should.equal(76)
    })
    it('converts "object-list" to 76', () => {
      bacnet.propertyKeyToNumber('object-list').should.equal(76)
    })
    xit('errors on "foo"', () => {
      try {
        bacnet.propertyKeyToNumber('foo')
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
    xit('errors on -1', () => {
      try {
        bacnet.propertyKeyToNumber(-1)
        should.fail('no error was thrown for invalid object type "foo"')
      } finally {}
    })
  })
})
