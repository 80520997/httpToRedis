/*
 * request.hpp
 *
 *  Created on: 2014年10月28日
 *      Author: houwei
 */
#ifndef HTTP_SERVER2_REQUEST_HPP
#define HTTP_SERVER2_REQUEST_HPP

#include <string>
#include <vector>
//#include "header.hpp"
#include <map>
#include <boost/asio.hpp>

namespace http {
namespace server2 {

/// A request received from a client.
struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  //std::vector<header> headers;
  std::map<std::string, std::string> header;
  std::map<std::string,std::string> parameters;
  std::string& get_param(std::string key,std::string def = NULL)
  {
	  return parameters.count(key) ? parameters[key] : def;
  }

};

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_REQUEST_HPP
