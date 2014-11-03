#ifndef __HIREDIS_BOOSTASIO_H__
#define __HIREDIS_BOOSTASIO_H__

#include <hiredis/hiredis.h>
#include <hiredis/async.h>

#include <iostream>
#include <string>
#include <stdio.h>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RedisBoostClient
{
public:
   RedisBoostClient(boost::asio::io_service& io_service);
   ~RedisBoostClient();
   typedef std::function<void(RedisBoostClient*,redisReply*)> CommandCallback;
	void connect(std::string, int);
	bool connected() const;
	int command(const CommandCallback& cb, std::string cmd);




private:
   redisAsyncContext *context_ = NULL;
	boost::asio::ip::tcp::socket socket_;
	bool read_requested_ = false;
	bool write_requested_ = false;
	bool read_in_progress_ = false;
	bool write_in_progress_ = false;

	void operate();
	void handle_read(boost::system::error_code ec);
   void handle_write(boost::system::error_code ec);
   static void add_read(void *privdata);
   static void del_read(void *privdata);
   static void add_write(void *privdata);
   static void del_write(void *privdata);
   static void cleanup(void *privdata);
   static void connectCallback(const redisAsyncContext* ac, int status);
   static void commandCallback(redisAsyncContext* ac, void*, void*);
   static  RedisBoostClient* getHiredis(const redisAsyncContext* ac);
   void commandCallback(redisReply* reply, CommandCallback* privdata);
   void connectCallback(int status);

};




#endif /*__HIREDIS_BOOSTASIO_H__*/
