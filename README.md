# experimental nodejs BACnet wrapper

This is an experiment to see if it is viable to build a thin wrapper around the
[bacnet-stack c library](http://bacnet.sourceforge.net/) using the Node.js addons api.

## Contents

- bacnet-stack/ - contains the full contents of the c library checked out from svn://svn.code.sf.net/p/bacnet/code/trunk/bacnet-stack
  - .svn/ - the svn db is checked in to make it easier to update to newer versions and in case we have something to push back to the c library - I don't know if this is a bad idea
- src/ - contains the wrapper code
  - h_*.c - request and response message handlers - modified from code in bacnet-stack/demo/handlers
  - functions.cc - contains the addon code that runs when functions are called from js
  - init.cc - runs on module initialisation
  - module.cc - defines the module
  - *conversion.cc - provides conversions between js and the BACnet structs
  - emitter.cc - emits js events into an EventEmitter in js-land asynchronously with events from the c code
- test/test.js - contains just a single example script which uses the module to send a whois message

## License

The bacnet-stack code is provided under an eCos license, which is a GPL license with the exception that code linked to
it is not covered. For simplicity we would release the wrapper also under the eCos license.

Some code is additionally licensed under an MIT license.

## Technical considerations

- Currently this builds for OSX and Linux (x86), it needs to be cross-platform. The c library supports most everything
  so it shouldn't be a problem.
- We need to consider that nodejs runs native code in a thread pool and the c code is probably designed to be single
  threaded - so Nodejs's concurrency controls will probably be relevant. The io in bip.h is blocking, I imagine having a
  thread for sending outgoing messages and a thread listening for incoming messages.
- We will want some high load tests to detect memory leaks and incorrect threading.
- To have really good automated integration tests, we'd need tocreate multiple internal interfaces which support 
  broadcast to each other so that multiple instances can use the same ports
- Some of the bacnet-stack code is static and so we are restricted to one instance per VM - the tests spawn various 
  devices in forked VMs 
- Many of the files I've included in the build are probably not needed and can be removed to reduce install time
- We may want to switch to a tag of the bacnet-stack instead of the random head on the day I started the project
- Segmentation is not supported in bacnet-stack (see below)
- I've seen failures occasionally when making requests concurrently - where possible 

## Installation

`npm install` should get everything you need. Though you may also need to install compiler tools - such as 
`build-essential` on Ubuntu.

## Operations needed

BACnet is a huge and impressive protocol suite - the scope of this project currently is only to cover the most basic 
operations which are needed for our use case and compliance.

For our use case we will need at least these commands:

- whois
- write property
- read property
- subscribe to cov

And to receive these messages:

- i am
- read ack
- write ack
- cov ack
- cov notification

In order to simulate devices we will also want these the opposite way around.

## Priorities remaining

6. synchronise sending requests
7. separate the code into modules
8. use c++11
9. Create cross-platform tests on travisci
9. Cross node versions
11. Write properties
12. Subscribe to COV
13. Try again to get 2 devices running on 1 process
14. Add stress tests to seek memory leaks / socket problems / queue exhaustion (events added faster than consumed in either direction)
15. Enable writing of the deviceid using WriteProperty - so it can be configured by the installer
16. Get cross-architecture tests running
17. Get cross-architecture to work
18. Make initialisation non-blocking

## Things we're not currently trying to support

- non-ip datalinks
- bbmp / remote device registration
- services other than those above
- bacnet routers

## Useful BACnet documentation

- [BACnet device IDs](http://kargs.net/BACnet/Foundations2012-BACnetDeviceID.pdf)
- [Segmentation in BACnet](http://www.chipkin.com/segementation-in-bacnet/)
- [The language of BACnet](http://www.bacnet.org/Bibliography/ES-7-96/ES-7-96.htm)


## Usage considerations

### Segmentation not supported

bacnet-stack doesnt support segmentation - so this client doesn't support segmentation. 

This can be worked around for large arrays (particularly object lists) by reading the length of the array (index 0) and 
then reading each element of the array. This should be OK if these arrays don't need to be subscribed using COV or read 
regularly.

Another option is that we find a way to get segmentation support, such as by merging this branch - 
https://svn.code.sf.net/p/bacnet/code/branches/jbennet/bacnet-stack-0-5-7/
