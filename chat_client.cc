//chat_client.cc
//~~~~~~~~~~~~~~
//
//Alberto Geroldi - 2019

#include "message.h"
#include "chat_client.h"

Chat_client::Chat_client(boost::asio::io_context& io_context, 
 //The ip::basic_resolver_results class template is used to 
 //define a range over the results returned by a resolver.
 const boost::asio::ip::tcp::resolver::results_type& endpoints)
 : io_context_(io_context), socket_(io_context) {
 do_connect(endpoints);
};

void Chat_client::write(const Chat_message& msg) {
//This function (boost::asio::post) submits an object 
//for execution using the specified executor. The function object 
//is queued for execution, and is never called 
//from the current thread prior to returning from post(). 
 boost::asio::post(io_context_,
  [this,msg]() {
   //queue not empty.
   bool write_in_progress = !write_messages_.empty();
   write_messages_.push_back(msg);
   //if it is empty, do_write() and send message to buffer.
   if (!write_in_progress) {
    do_write();
   }
  });
};

void Chat_client::close() {
 boost::asio::post(io_context_, [this]() { socket_.close(); });
};

void Chat_client::do_connect( const boost::asio::ip::tcp::resolver::results_type& endpoints) {
 //The socket is automatically opened if it is not already open. If the connect fails, 
 //and the socket was automatically opened, the socket is not returned to the closed state.
 boost::asio::async_connect(socket_, endpoints,
  //On success, the successfully connected endpoint.
  //Otherwise, a default-constructed endpoint.
  [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
   if (!ec) {
    //std::cout << "here" ; //read only one time, when connection succed!
    do_read_header();
   }
  });
};

void Chat_client::do_read_header() {
 boost::asio::async_read(socket_,
 //Read the fixed-length header of the next message from the server.
 //chat_message::header_length call an enum value
 //Although the buffers object may be copied as necessary, 
 //ownership of the underlying memory blocks is retained by the caller, 
 //which must guarantee that they remain valid until the handler is called.
  boost::asio::buffer(read_msg_.data(), Chat_message::header_length),
  [this](boost::system::error_code ec, std::size_t /*length*/) {
   if (!ec && read_msg_.decode_header()) {
    do_read_body();
   }
   else {
    socket_.close();
   }
  });
};

void Chat_client::do_read_body() {
 boost::asio::async_read(socket_,
  boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
  [this](boost::system::error_code ec, std::size_t /*length*/) {
   if (!ec) {
    //This read buffer from server that host the service
    std::cout.write(read_msg_.body(), read_msg_.body_length()) << '\n';
    std::cout << '\n';
    do_read_header();
   }
   else {
    socket_.close();
   }
  });
};

void Chat_client::do_write() {
 boost::asio::async_write(socket_,
 //write_msg_ of type std::deque , a container 
 //that allows fast insertion and deletion at both its beginning and its end.
  boost::asio::buffer(write_messages_.front().data(), 
                      write_messages_.front().length()),
  [this](boost::system::error_code ec, std::size_t /*length*/) { 
   if (!ec) {
    write_messages_.pop_front();
    
    if (!write_messages_.empty()) {
     do_write();
    }
   }
   else {
    socket_.close();
   }
  });
};

//return the complete message sent in the chat room by chat participant
std::string Chat_client::complete_message(std::string name,std::string message ) {
 std::string complete = name + ":" + " " + message;
 return complete;
};
