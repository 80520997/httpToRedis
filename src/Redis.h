#pragma once
#include<memory>
#include<vector>
#include<string>
#include<hiredis/hiredis.h>
namespace redis{
	typedef std::shared_ptr<std::vector<std::string>> hlist;

class Redis
{
public:
	Redis();
	
	bool connect(std::string host,int port);

	std::string get(std::string key);
	bool set(std::string key, std::string value);

	~Redis();


private:
	redisContext *context_ = nullptr;
	redisReply *reply_ = nullptr;

};

}

