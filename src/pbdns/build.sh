#!/bin/sh
#This is a primitive build script. Given that currently Ubuntu 10.4 LTS server 64 bit is our only target platform this should be OK for now.
#In the future we may want to move the build stuff to the cmake build system.
g++ main.cpp UnPrivileged.cpp MainServer.cpp PbRoutingCore.cpp DnsForwarder.cpp jsonmeimpl/PbrConfigFactory.cpp jsonmeimpl/JsonMeConfig.cpp jsonmeimpl/JsonMeWsnetConfig.cpp DnsResponse.cpp -lboost_system -lboost_regex -ljsonme -o pbdnsd
#g++ core_test_main.cpp ./jsonmeimpl/PbrConfigFactory.cpp ./jsonmeimpl/JsonMeConfig.cpp ./jsonmeimpl/JsonMeWsnetConfig.cpp  PbRoutingCore.cpp -I/usr/local/include -ljsonme -lboost_system -o core_test
