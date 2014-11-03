//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_parser.hpp"
#include "request.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

namespace http {
namespace server2 {

request_parser::request_parser()
  : state_(method_start)
{
}

void request_parser::reset()
{
  state_ = method_start;
}

boost::tribool request_parser::each_str(request& request_, char* begin, std::size_t len)
{
	request_str_.append(begin,len);
	auto index = request_str_.find("\r\n\r\n");
	if(index == std::string::npos)
	{
		return false;
	}

	auto next = request_str_.find("\r\n");
	//获取请求模式和URL地址
	if(!init_one_line(request_,request_str_.substr(0,next)))
		return boost::indeterminate;

	//解析和初始化GET参数
	if(!init_param(request_))
		return boost::indeterminate;
	//获取头信息
	if(!init_header(request_,request_str_.substr(next + 2,index - 6)))
		return boost::indeterminate;
	if(!request_.header.count("Content-Length") || request_.method != "POST")
		return true;

	/* 后续写POST实现 */


	return true;

}

bool request_parser::init_one_line(request& request_,std::string str)
{
	std::vector<std::string> s;
	boost::split(s,str,boost::is_any_of(" "));
	if(s.size() != 3)
		return false;
	request_.method = s[0];
	request_.uri = s[1];
	return true;
}

bool request_parser::init_param(request& request_)
{


	auto index = request_.uri.find("?");
	if(index == std::string::npos)
		return true;

	auto gets = request_.uri.substr(index + 1);
	if(!gets.length())
		return true;

	std::vector<std::string> s;
	boost::split(s,gets,boost::is_any_of("&"));

	for(auto iter = s.begin();iter !=s.end(); ++iter)
	{

		auto index = iter->find("=");
		if(index == std::string::npos)
			continue;
		//usleep(50);
		//std::cout << "key:" << key << "##val:" << val << std::endl;
		//std::cout << "##iter:" << *iter << "$$=" << index << "%%key:" << key << "&&val:" << val << std::endl;

		request_.parameters.insert(std::pair<std::string, std::string>(std::move(iter->substr(0,index)), std::move(url_decode(iter->substr(index +1)))));

	}


	return true;
}


bool request_parser::init_header(request& request_,std::string str)
{

	std::string key;
	std::string val;
	bool is_key = true;
	for(auto iter = str.begin();iter != str.end(); ++iter)
	{
		if(*iter == '\r')
			continue;

		if(*iter == '\n')
		{
			is_key = true;
			if(key.length())
				request_.header.insert(std::pair<std::string, std::string>(key,val));
			key = "";
			val = "";
			continue;
		}
		else if(*iter == ':')
		{
			is_key = false;
			continue;
		}
		is_key ? key.append(1,*iter) : val.append(1,*iter);
	}

	if(key.length())
		request_.header.insert(std::pair<std::string, std::string>(key,val));

	return true;

}


std::string request_parser::url_decode(const std::string& in)
{
  std::string out;
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return "";
        }
      }
      else
      {
        return "";
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return out;
}



bool request_parser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c)
{
  switch (c)
  {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}

} // namespace server2
} // namespace http
