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

- Currently this only builds for osx, it needs to be cross-platform. The c library supports more everything so it shouldn't be a problem
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
9. Read list of objects
3. Create cross-platform tests on travisci
4. Get cross-platform to work
5. Get cross-architecture tests running
6. Get cross-architecture to work
6. Cross node versions
7. Add configuration for port, interface, device number, vendor id, etc.
8. Improve tests (try to get 2 devices running under the same process)
10. Read properties
11. Write properties
12. Subscribe to COV
13. Add stress tests to seek memory leaks / socket problems / queue exhaustion (events added faster than consumed in either direction)
14. Try again to get 2 devices running on 1 process 
15. Enable writing of the deviceid using WriteProperty - so it can be configured by the installer
16. use c++11
17. separate the code into modules
