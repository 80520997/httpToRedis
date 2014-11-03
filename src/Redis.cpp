//#include<boost/lexical_cast.hpp>
#include<hiredis/hiredis.h>
#include "Redis.h"
#include <iostream>

using namespace redis;
Redis::Redis()
{
}
bool Redis::connect(std::string host, int port)
{
	struct timeval timeout = { 1, 500000 };
	context_ = redisConnectWithTimeout(host.c_str(), port, timeout);
	if (context_ == NULL || context_->err )
	{
		return false;
	}
	return true;

}

bool Redis::set(std::string key, std::string value)
{

	//std::string val = std::move(boost::lexical_cast<std::string>(value));

	reply_ = static_cast<redisReply*>(redisCommand(context_, "SET %s %s",key.c_str(),value.c_str()));

	//freeReplyObject(reply_);

	return true;
	
	
}

std::string Redis::get(std::string key)
{
	reply_  = static_cast<redisReply*>(redisCommand(context_, "GET %s",key.c_str()));

	std::string str(reply_->str, reply_->len);
	//freeReplyObject(reply_);

	return str;
}



Redis::~Redis()
{
	if(context_)
		redisFree(context_);
}
