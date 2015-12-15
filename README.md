# nodejs BACnet wrapper prototype

This is a prototype to see if it is viable to build a thin wrapper around the
[bacnet c library](http://bacnet.sourceforge.net/) using the Node.js addons api.

## Contents

- bacnet-stack/ - contains the full contents of the c library checked out from svn://svn.code.sf.net/p/bacnet/code/trunk/bacnet-stack
  - .svn/ - the svn db is checked in to make it easier to update to newer versions and in case we have something to push back to the c library - I don't know if this is a bad idea
- src/ - contains the wrapper code
  - basicwhois.* - sends a whois message
  - functions.* - contains the addon code that runs when the whois method is called from js
  - init.* - runs on module initialisation
  - module.cc - defines the module
- test/test.js - contains just a single example script which uses the module to send a whois message

## License

The bacnet-stack code is provided under an eCos license, which is a GPL license with the exception that code linked to
it is not covered. For simplicity we would probably release the wrapper also under the eCos license.

## Technical considerations

- Currently this builds for OSX and Linux (x86), it needs to be cross-platform. The c library supports most everything
  so it shouldn't be a problem.  To build on Ubuntu you need to `sudo apt-get install build-essential`.
- Everyone agrees that we need integration tests for the wrapper - we need a high confidence about it's stability.
- To be low level, the commands will be fire-and-forget, the responses will come back through an event emitter - we
  haven't looked at the api for those yet.
- We need to consider that nodejs runs native code in a thread pool and the c code is probably designed to be single
  threaded - so Nodejs's concurrency controls will probably be relevant. The io in bip.h is blocking, I imagine having a
  thread for sending outgoing messages and a thread listening for incoming messages. An alternative is to use libuv but
  that might involve more changes than just writing a port as the callers of bip may require blocking
- We will want some high load tests to detect memory leaks and incorrect threading.
- It may be possible to test stuff on the loopback interface - in which case it would be useful if we can instantiate
  multiple devices on different ports.
- Many of the files I've included in the build are probably not needed and can be removed to reduce install time
- We may want to switch to a tag of the bacnet-stack instead of the random head on the day I started the project
-

## Operations needed

For our adapter we will need at least these commands:

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

## Riskiest assumptions

- we can get it to work on arm and x86
- we can get it to work on osx and linux (and windows)
- we can get adequate control from js to this c code which is written to be modified for purpose as a bacnet device
- we can make split synchronous c code and make calls to js in between

## Priority

0. Send Whois messages
1. Respond to whois messages
2. Receive iam messages and translate to js (events?)
3. Create an integration test setup
4. Add configuration for port, interface, device number, vendor id, etc.
5. Read list of objects
6. Improve tests (try to get 2 devices running under the same process)
7. separate the code into modules
8. use c++11
9. Create cross-platform tests on travisci
9. Cross node versions
10. Read properties
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