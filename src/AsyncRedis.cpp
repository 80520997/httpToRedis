/*
 * AsyncRedis.cpp
 *
 *  Created on: 2014年10月16日
 *      Author: houwei
 */
#include<hiredis/async.h>
#include<iostream>
#include<string>
#include<functional>
#include<boost/bind.hpp>
#include "AsyncRedis.h"

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = static_cast<redisReply*>(r);
    if (reply == NULL) return;
    	printf("argv[%s]: %s\n", (char*)privdata, reply->str);

    /* Disconnect after receiving the reply to GET */
    redisAsyncDisconnect(c);
}

namespace redis {

AsyncRedis::AsyncRedis(boost::asio::io_service& io_service):redisClient_(io_service)
{

}

void AsyncRedis::connect(std::string host,int port,std::function<void(bool,std::string)> fun)
{

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);

	redisClient_.asyncConnect(endpoint,fun);


}

/*
std::string AsyncRedis::toString(redisReply* reply)
{
	if(reply == NULL)
		return "";

	switch(reply->type)
	{
		case REDIS_REPLY_STRING:
			return reply->str;
		case REDIS_REPLY_INTEGER:
			return std::to_string(reply->integer);
	}
	return "";
}
*/


void AsyncRedis::set(std::function<void(bool status)> callbak,std::string key, std::string val){
/*
	auto f = [&callbak](RedisBoostClient* asyncRedis, redisReply* reply){

		std::cout << reply->str << std::endl;
		std::cout << reply->elements << std::endl;
		std::cout << reply->integer << std::endl;
		std::cout << reply->len << std::endl;
		std::cout << reply->type << std::endl;


		(std::string(reply->str) == "OK") ? callbak(true) : callbak(false);
	};
	std::string cmd;
	cmd.append("SET ").append(key).append(" ").append(val);
	//redisBoostClient_.command(f,cmd.c_str());
	*/
}

void AsyncRedis::get(std::function<void(std::string)> callbak,std::string key){

	/*
	auto f = [callbak,this](RedisBoostClient* asyncRedis, redisReply* reply){
		callbak(std::move(toString(reply)));
	};
	std::string cmd;
	cmd.append("GET ").append(key);
	redisBoostClient_.command(f,cmd.c_str());
	*/

}


void AsyncRedis::incr(std::function<void(std::string)> callbak,std::string key){
	auto f = [callbak,this](const RedisValue &value){
		std::cout << "result:" << value.toInt() << std::endl;
		callbak(std::to_string(value.toInt()));
		//freeReplyObject(reply);
	};
	//std::string cmd;
	//cmd.append("INCR ").append(key);
	redisClient_.command("incr", key, f);
}




AsyncRedis::~AsyncRedis() {

	//redisClient_.~RedisClient();
	std::cout << "redis 释放" << std::endl;


}

} /* namespace redis */
