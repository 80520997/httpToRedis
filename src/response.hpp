/*
 * response.hpp
 *
 *  Created on: 2014年10月28日
 *      Author: houwei
 */

#ifndef SRC_RESPONSE_HPP_
#define SRC_RESPONSE_HPP_

#include <boost/noncopyable.hpp>


namespace http {
namespace server2 {
class response : public boost::noncopyable
{
 public:
  enum HttpStatusCode
  {
    kUnknown,
    k200Ok = 200,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k404NotFound = 404,
  };

  explicit response(bool close)
    : statusCode_(kUnknown)
  {
  }

  void setStatusCode(HttpStatusCode code)
  { statusCode_ = code; }

  void setStatusMessage(const std::string& message)
  { statusMessage_ = message; }



  // FIXME: replace string with StringPiece
  void setHeader(const std::string& key, const std::string& value)
  { headers_[key] = value; }

  void setBody(const std::string& body)
  { body_ = body; }


 private:
  std::map<std::string, std::string> headers_;
  HttpStatusCode statusCode_;
  // FIXME: add http version
  std::string statusMessage_;

  std::string body_;
};



}
}



#endif /* SRC_RESPONSE_HPP_ */
