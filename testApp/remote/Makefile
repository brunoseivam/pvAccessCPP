# This is a Makefile fragment, see ../Makefile

SRC_DIRS += $(PVACCESS_TEST)/remote

TESTPROD_HOST += testChannelAccess
testChannelAccess_SRCS = channelAccessIFTest.cpp
testHarness_SRCS += channelAccessIFTest.cpp
TESTS += testChannelAccess

TESTPROD_HOST += testCodec
testCodec_SRCS = testCodec
testHarness_SRCS += testCodec.cpp
testCodec_SYS_LIBS_WIN32 += ws2_32
TESTS += testCodec

TESTPROD_HOST += testRPC
testRPC_SRCS += testRPC.cpp
TESTS += testRPC

TESTPROD_HOST += testRemoteClientImpl
testRemoteClientImpl_SRCS += testRemoteClientImpl.cpp

TESTPROD_HOST += testChannelConnect
testChannelConnect_SRCS += testChannelConnect.cpp

TESTPROD_HOST += testServerContext
testServerContext_SRCS += testServerContext.cpp
TESTS += testServerContext

TESTPROD_HOST += testmonitorfifo
testmonitorfifo_SRCS += testmonitorfifo.cpp
TESTS += testmonitorfifo

TESTPROD_HOST += testsharedstate
testsharedstate_SRCS += testsharedstate.cpp
TESTS += testsharedstate

PROD_HOST += testServer
testServer_SRCS += testServer.cpp

PROD_HOST += testGetPerformance
testGetPerformance_SRCS += testGetPerformance.cpp

PROD_HOST += testMonitorPerformance
testMonitorPerformance_SRCS += testMonitorPerformance.cpp

PROD_HOST += rpcServiceExample
rpcServiceExample_SRCS += rpcServiceExample.cpp

PROD_HOST += rpcServiceAsyncExample
rpcServiceAsyncExample_SRCS += rpcServiceAsyncExample.cpp

PROD_HOST += rpcWildServiceExample
rpcWildServiceExample_SRCS += rpcWildServiceExample.cpp

PROD_HOST += rpcClientExample
rpcClientExample_SRCS += rpcClientExample.cpp

PROD_HOST += pipelineServiceExample
pipelineServiceExample_SRCS += pipelineServiceExample.cpp

TESTPROD_HOST += testClientFactory
testClientFactory_SRCS += testClientFactory.cpp

TESTPROD_HOST += testRAII
testRAII_SRCS += testRAII.cpp
