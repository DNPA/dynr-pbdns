JSONIMPLCFILES = jsonmeimpl/PbrConfigFactory.cpp jsonmeimpl/JsonMeConfig.cpp jsonmeimpl/JsonMeWsnetConfig.cpp
JSONIMPLHFILES = jsonmeimpl/JsonMeConfig.hpp  jsonmeimpl/JsonMeWsnetConfig.hpp
JSONHFILES = PbrConfigFactory.hpp AbstractPbrConfig.hpp AbstractWsnetConfig.hpp NullWsnetConfig.hpp PbrConfigException.hpp Peer.hpp
ROUTINGCORECFILES = PbRoutingCore.cpp
ROUTINGCOREHFILES = PbRoutingCore.hpp
SERVERCFILES = main.cpp UnPrivileged.cpp MainServer.cpp DnsForwarder.cpp DnsResponse.cpp
SERVERHFILES = UnPrivileged.hpp MainServer.hpp DnsForwarder.hpp DnsResponse.hpp cachemap.hpp 
CFILES = $(JSONIMPLCFILES) $(ROUTINGCORECFILES) $(SERVERCFILES)
HFILES = $(JSONIMPLHFILES) $(JSONHFILES) $(ROUTINGCOREHFILES) $(SERVERHFILES)
LIBFLAGS = -lboost_system -lboost_regex -ljsonme

all: pbdnsd

pbdnsd: $(CFILES) $(HFILES)
	g++ $(CFILES) $(LIBFLAGS) -o pbdnsd


