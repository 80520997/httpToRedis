/*
 * connection.cpp
 *
 *  Created on: 2014年10月28日
 *      Author: houwei
 */

#include "connection.hpp"
#include <vector>
#include <iostream>
#include <thread>
#include <boost/bind.hpp>
#include "AsyncRedis.h"


namespace http {
namespace server2 {

connection::connection(boost::asio::io_service& io_service)
  : socket_(io_service),io_service_(io_service),asyncRedis(io_service)
{

}

boost::asio::ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
	std::cout << "start(); "  << std::endl;

	socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void connection::handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  if (!e)
  {
    /*
	 boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);
     */

	  //std::cout  <<"###" <<  std::endl;

	  boost::tribool result;
	  result = request_parser_.each_str(request_ ,buffer_.data(), bytes_transferred);




	  if (result)
	     {

		  for(auto iter = request_.header.begin();iter != request_.header.end();++iter)
		  {
			  //std::cout << iter->first <<"###" <<  iter->second << std::endl;
		  }

		  for(auto iter = request_.parameters.begin();iter != request_.parameters.end();++iter)
		  {
			  //std::cout << iter->first <<"###" <<  iter->second << std::endl;
		  }

		  auto self(shared_from_this());
		  asyncRedis.connect("127.0.0.1",6379,[self,this](bool is,std::string msg){std::cout << msg << std::endl;on_req();});

		  //std::cout << "on_req(); "  << std::endl;


	     }
	     else if (!result)
	     {
	    	 socket_.async_read_some(boost::asio::buffer(buffer_),
	           boost::bind(&connection::handle_read, shared_from_this(),
	             boost::asio::placeholders::error,
	             	 boost::asio::placeholders::bytes_transferred));
	     }
	     else
	     {
	       boost::asio::async_write(socket_, boost::asio::buffer("HTTP/1.1 400 Bad Request\r\n\r\n"),
	           boost::bind(&connection::handle_write, shared_from_this(),
	             boost::asio::placeholders::error));
	     }

  }


}

void connection::on_req()
{

	//boost::asio::io_service io_service;


	//asyncRedis.set([](bool status){ std::cout << (status ? "ok" : "error") << std::endl; },"aaaa","lllll");

	//auto self(shared_from_this());
	asyncRedis.incr(boost::bind(&connection::write_body,shared_from_this(),_1) ,"aaaa");

	//write_body("aaaaaaaaaaa");



}
void connection::write_body(std::string str)
{
	std::cout << "ssssssss" << str << "ssss" <<  std::endl;

	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Server: hserver\r\n";
	header += "Content-Type: text/html; charset=utf-8 \r\n";
	header += "Connection: close\r\n";
	header += "Content-Length: " + std::to_string(str.length()) + "\r\n\r\n";
	header += str;

	boost::asio::async_write(socket_, boost::asio::buffer(header),
		           boost::bind(&connection::handle_write, shared_from_this(),
		             boost::asio::placeholders::error));

}


void connection::handle_write(const boost::system::error_code& e)
{
  if (!e)
  {
	  // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  std::cout << "connect end " << std::endl;


}

} // namespace server2
} // namespace http
