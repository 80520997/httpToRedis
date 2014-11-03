/*
 * main.cpp
 *
 *  Created on: 2014年10月28日
 *      Author: houwei
 */
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"
#include "AsyncRedis.h"
/*
using namespace redis;

void oo(bool status)
{
	std::cout << status << std::endl;
}
*/
int main(int argc, char* argv[]) {

	/*
	boost::asio::io_service io_service;


	AsyncRedis asyncRedis(io_service);

	asyncRedis.connect("183.136.160.211",9903);

	asyncRedis.set([](bool status){ std::cout << (status ? "ok" : "error") << std::endl; },"aaaa","pppp");

	asyncRedis.get([](std::string str){ std::cout << str << std::endl; },"aaaa");
	 */
	/*
	rl.command([](redisBoostClient* a,redisReply* b){},"SET aaaa cccc");
	rl.command([](redisBoostClient* a,redisReply* b){},"SET aaaa sss");
	rl.command([](redisBoostClient* a,redisReply* b){},"SET aaa 5555");
	*/


	//io_service.run();


	try {

		http::server2::server s("0.0.0.0", "8888", "/home/houwei", 1);

		s.run();
	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
