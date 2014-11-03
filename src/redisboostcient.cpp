#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <functional>
#include <boost/bind.hpp>
#include "redisboostclient.h"



RedisBoostClient::RedisBoostClient(boost::asio::io_service& io_service)
: socket_(io_service)
{


}
RedisBoostClient::~RedisBoostClient()
{
	if(context_)
	{
		std::cout << "释放" << std::endl;
		::redisAsyncFree(context_);
	}
}

void RedisBoostClient::connect(std::string add, int port)
{
	auto i = 0;

	do
	{
		if(context_)
			::redisAsyncFree(context_);
		context_ = ::redisAsyncConnect(add.c_str(), port);
		++i;

	}//链接失败重连
	while(context_->c.fd < 0 && i < 5);

	try{
		socket_.assign(boost::asio::ip::tcp::v4(),context_->c.fd);
	}catch(std::exception& ec) {
	 	std::cout << ec.what() << std::endl;
	}
	std::cout << "fid:" << context_->c.fd << std::endl;


	/*register hooks with the hiredis async context*/
	context_->ev.addRead = add_read;
	context_->ev.delRead = del_read;
	context_->ev.addWrite = add_write;
	context_->ev.delWrite = del_write;
	context_->ev.cleanup = cleanup;
	context_->ev.data = this;

	assert(context_->onConnect == NULL);
	assert(context_->onDisconnect == NULL);
	/*I don't know whether this even gets used, but...*/
	//context_->ev.data = context_;
	::redisAsyncSetConnectCallback(context_, connectCallback);
	/*C wrapper functions will use this
	 *pointer to call class members.
	 *fortunately hiredis doesn't use the data field.
	 *see line 70 of async.h
	 */
	//context_->data = this;
}

bool RedisBoostClient::connected() const
{
	return context_ && (context_->c.flags & REDIS_CONNECTED);
}



void RedisBoostClient::operate()
{
	if(read_requested_ && !read_in_progress_) {
		read_in_progress_ = true;
		socket_.async_read_some(boost::asio::null_buffers(),
				boost::bind(&RedisBoostClient::handle_read,this,boost::asio::placeholders::error));
	}

	if(write_requested_ && !write_in_progress_) {
		write_in_progress_ = true;
		socket_.async_write_some(boost::asio::null_buffers(),
				boost::bind(&RedisBoostClient::handle_write,this,boost::asio::placeholders::error));
	}
}
	
void RedisBoostClient::handle_read(boost::system::error_code ec)
{

	if(!read_in_progress_)
		read_in_progress_ = false;

	if(!ec) {
		redisAsyncHandleRead(context_);
	}

	if (!ec || ec == boost::asio::error::would_block)
		operate();
}

void RedisBoostClient::handle_write(boost::system::error_code ec)
{
	write_in_progress_ = false;
	if(!ec) {
		redisAsyncHandleWrite(context_);
	}

	if (!ec || ec == boost::asio::error::would_block)
		operate();
}

/* static */ void RedisBoostClient::add_read(void *privdata)
{
	RedisBoostClient *C = static_cast<RedisBoostClient*>(privdata);
	C->read_requested_ = true;
	C->operate();
}

/* static */ void RedisBoostClient::del_read(void *privdata)
{
	RedisBoostClient *C = static_cast<RedisBoostClient*>(privdata);
	C->read_requested_ = false;
}

/* static */ void RedisBoostClient::add_write(void *privdata)
{
	RedisBoostClient *C = static_cast<RedisBoostClient*>(privdata);
	C->write_requested_ = true;
	C->operate();

}

void RedisBoostClient::del_write(void *privdata)
{
	RedisBoostClient *C = static_cast<RedisBoostClient*>(privdata);
	C->write_requested_ = false;
}

void RedisBoostClient::cleanup(void *privdata)
{
	//redisAsyncContext *ac = static_cast<redisAsyncContext*>(privdata);
	//RedisBoostClient *C = static_cast<RedisBoostClient*>(ac->data);
	/*Do I even need this?*/
	printf("cleanup called...\n");
}

int RedisBoostClient::command(const CommandCallback& cb, std::string cmd)
{
	/*
	if (!connected())
	{
		std::cerr << "redis connect fais" << std::endl;
		return REDIS_ERR;

	}
	 */
  CommandCallback* p = new CommandCallback(cb);
  std::cout << cmd << std::endl;
  int ret = ::redisAsyncCommand(context_, commandCallback, p, cmd.c_str());

  std::cout << "result:" << ret << std::endl;

  return ret;
}

/* static */ void RedisBoostClient::connectCallback(const redisAsyncContext* ac, int status)
{

	getHiredis(ac)->connectCallback(status);
}

/* static */ RedisBoostClient* RedisBoostClient::getHiredis(const redisAsyncContext* ac)
{
	RedisBoostClient* hiredis = static_cast<RedisBoostClient*>(ac->ev.data);
	assert(hiredis->context_ == ac);
	return hiredis;
}


void RedisBoostClient::connectCallback(int status)
{
  if (status != REDIS_OK)
  {
    std::cout << context_->errstr << " failed to connect to " << std::endl;
  }

  std::cout << "connect ok" << std::endl;
  /*
  else
  {

  }

  if (connectCb_)
  {
    connectCb_(this, status);
  }
  */
}


/* static */ void RedisBoostClient::commandCallback(redisAsyncContext* ac, void* r, void* privdata)
{
  redisReply* reply = static_cast<redisReply*>(r);
  CommandCallback* cb = static_cast<CommandCallback*>(privdata);
  //auto* hiredis = static_cast<RedisBoostClient*>(ac->data);

  getHiredis(ac)->commandCallback(reply, cb);
  //hiredis->commandCallback(reply, cb);
}

void RedisBoostClient::commandCallback(redisReply* reply, CommandCallback* cb)
{
	(*cb)(this, reply);
	delete cb;
}


