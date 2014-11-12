/*
 * AsyncRedis.h
 *  非阻赛
 *
 *  Created on: 2014年10月16日
 *      Author: houwei
 */

#ifndef ASYNCREDIS_H_
#define ASYNCREDIS_H_

#include <functional>
#include <assert.h>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
//#include <hiredis/async.h>
//#include "redisboostclient.h"
#include <redisclient/redisclient.h>



namespace redis {

class AsyncRedis : public boost::noncopyable {
public:
	AsyncRedis(boost::asio::io_service& io_service);
	virtual ~AsyncRedis();
	void connect(std::string, int,std::function<void(bool,std::string)>);
	void set(std::function<void(bool status)> callbak,std::string key, std::string val);
	void get(std::function<void(std::string)> callbak,std::string key);
	void incr(std::function<void(std::string)> callbak,std::string key);
private:
	//typedef std::function<void(redisReply*)> RunCallback;
	//std::string toString(redisReply*);


	RedisClient redisClient_;

	//RedisBoostClient redisBoostClient_;





};

} /* namespace redis */

#endif /* ASYNCREDIS_H_ */
