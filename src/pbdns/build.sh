#!/bin/sh
g++ main.cpp UnPrivileged.cpp MainServer.cpp PbRoutingCore.cpp DnsForwarder.cpp jsonmeimpl/PbrConfigFactory.cpp jsonmeimpl/JsonMeConfig.cpp jsonmeimpl/JsonMeWsnetConfig.cpp DnsResponse.cpp -lboost_system -lboost_regex -ljsonme -o pbdnsd
#g++ core_test_main.cpp ./jsonmeimpl/PbrConfigFactory.cpp ./jsonmeimpl/JsonMeConfig.cpp ./jsonmeimpl/JsonMeWsnetConfig.cpp  PbRoutingCore.cpp -I/usr/local/include -ljsonme -lboost_system -o core_test
