#ifndef _PBDNS_DNS_RESPONSE_HPP
#define _PBDNS_DNS_RESPONSE_HPP
#include <boost/array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
class DnsResponse {
     boost::array<char,4> mTrueIp;
      boost::array<char,4> mFalseIp;
   public: 
     DnsResponse(std::string trueip, std::string falseip);
     boost::array<char,65536 > reply(boost::array<char,65536 > &query ,std::size_t qsize,bool val);
};
#endif
