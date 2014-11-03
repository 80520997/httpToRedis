/*
 * connection.hpp
 *
 *  Created on: 2014年10月28日
 *      Author: houwei
 */
#ifndef HTTP_SERVER2_CONNECTION_HPP
#define HTTP_SERVER2_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "request.hpp"
#include "response.hpp"
#include "request_parser.hpp"
#include "AsyncRedis.h"

namespace http {
namespace server2 {

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit connection(boost::asio::io_service& io_service);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
      std::size_t bytes_transferred);

  void on_req();
  void write_body(std::string str);
  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e);

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  boost::asio::io_service& io_service_;


  /// The handler used to process the incoming request.

  /// Buffer for incoming data.
  boost::array<char, 8192> buffer_;



  /// The incoming request.
  request request_;


  redis::AsyncRedis asyncRedis;



  //response response_;

  /// The parser for the incoming request.
  request_parser request_parser_;


};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_CONNECTION_HPP
