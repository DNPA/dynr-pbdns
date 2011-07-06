#!/bin/sh
g++ main.cpp ./jsonmeimpl/PbrConfigFactory.cpp ./jsonmeimpl/JsonMeConfig.cpp ./jsonmeimpl/JsonMeWsnetConfig.cpp  -I/usr/local/include -ljsonme -lboost_system -o testme
