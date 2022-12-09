//server.cc
//~~~~~~~~~~~~~~~
//
//Author: Alberto Geroldi
//
//Description: base class to run server of the client-server part of the chat.
//Note that this file is compiled separate from the others --> g++ -pthread chat_server.cc -o chat_server
//You can run ./chat_server with multiple number port es: ./chat_server 45000 45054 ...

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "message.h"

typedef std::deque<Chat_message> chat_message_queue;

////////////////////////////////
//chat_participant
///////////////////////////////
class chat_participant
{
public:
 virtual ~chat_participant() {}
 virtual void deliver(const Chat_message& msg) = 0;
};
//every participant shares this pointer
typedef std::shared_ptr<chat_participant> chat_participant_ptr;

///////////////////////////////
//chat_room
//////////////////////////////

//Join the chat room to write a message visibles to everyone
//Server can handle multiple chat_room, one for each port number
class chat_room
{
public:
 void join(chat_participant_ptr participant) {
  //here participant shared pointer join the participants set of unique participant.
  participants_.insert(participant);

  for (auto msg: recent_msgs_) {
   participant->deliver(msg);
  }

 }

 //important to handle shared pointer also in async_read in chat participant, called in a lambda with self.
 void leave(chat_participant_ptr participant) {
  participants_.erase(participant);
 }

 void deliver(const Chat_message& msg) {
  recent_msgs_.push_back(msg);
  while (recent_msgs_.size() > max_recent_msgs) {
   recent_msgs_.pop_front();
  }
  for (auto participant: participants_) {
   participant->deliver(msg);
  }
 }

private:
 //std::set is an associative container that 
 //contains a sorted set of unique objects of type chat_participant_ptr.
 //It’s DOESN'T ALLOW duplicate elements
 //Uniqueness is determined by using the equivalence relation.
 //Can only contain elements of type chat_participant_ptr
 std::set<chat_participant_ptr> participants_;
 enum { max_recent_msgs = 100 };
 //recent_msgs of type std::deque
 chat_message_queue recent_msgs_;
};

//////////////////////////////////
//chat_session
/////////////////////////////////

class chat_session
 : public chat_participant,
   //allows an object chat_session that is currently managed by a std::shared_ptr (in this case chat_participant_ptr)
   //to safely generate additional std::shared_ptr instances that all share ownership of chat_session with chat_participant_ptr. 
   public std::enable_shared_from_this<chat_session>
{
public:
 chat_session(boost::asio::ip::tcp::socket socket, chat_room& room)
  : socket_(std::move(socket)),
    room_(room)//chat_room&
  {
  }

 void start() {
  //std:.shared_ptr participant join chat_room
  room_.join(shared_from_this());
  do_read_header();
 }

 void deliver(const Chat_message& msg) {
  bool write_in_progress = !write_msgs_.empty();
  write_msgs_.push_back(msg);
  if (!write_in_progress) {
   do_write();
  }
 }

private:
 void do_read_header() {
  //auto self(shared_from_this()) --> This is done in order to make sure that connection object outlives the asynchronous operation: 
  //as long as the lambda is alive (the async. operation is in progress), the connection instance is alive as well.
  //A common pattern for the "capture a strong reference to yourself" is to capture both a strong reference and a raw this. 
  //The strong reference keeps the this alive, and you use the this for convenient access to members.
  //NEED TO EXTEND OBJECT LIFETIME!
  auto self(shared_from_this());
  boost::asio::async_read(socket_,
   boost::asio::buffer(read_msg_.data(), Chat_message::header_length),
   [this, self](boost::system::error_code ec, std::size_t /*length*/) {
    if (!ec && read_msg_.decode_header()) {
     do_read_body();
    }
    else {
     room_.leave(shared_from_this());
    }
   });
 }

 void do_read_body() {
  auto self(shared_from_this());
  boost::asio::async_read(socket_,
   boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
   [this, self](boost::system::error_code ec, std::size_t /*length*/) {
    if (!ec) {
     room_.deliver(read_msg_);
     do_read_header();
    }
    else {
     room_.leave(shared_from_this());
    }
   });
 }

 void do_write() {
  auto self(shared_from_this());
  boost::asio::async_write(socket_,
   boost::asio::buffer(write_msgs_.front().data(),
   write_msgs_.front().length()),
   [this, self](boost::system::error_code ec, std::size_t /*length*/) {
    if (!ec) {
     write_msgs_.pop_front();
     if (!write_msgs_.empty()) {
      do_write();
     }
    }
    else {
     room_.leave(shared_from_this());
    }
   });
 }

private:
 boost::asio::ip::tcp::socket socket_;
  chat_room& room_;
  Chat_message read_msg_;
  chat_message_queue write_msgs_;
};

////////////////////////////////
//chat_server
///////////////////////////////

class chat_server {

public:
 chat_server(boost::asio::io_context& io_context,
  const boost::asio::ip::tcp::endpoint& endpoint)
  : acceptor_(io_context, endpoint) {
   do_accept();
 }

private:
 //listen for new connection
 void do_accept() {
  //waiting for connection (handshake)
  //after connection async_read and async_write will be executed and connection will be closed but
  //server continues to running execpt explicit shutdown (is an async server!).
  acceptor_.async_accept( 
   [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
    if (!ec) {
     //constructor for accepting connection from client
     //the same as for example: auto session = std::shared_ptr<chat_session>(new chat_session(socket, room_)); session->start();
     std::make_shared<chat_session>(std::move(socket), room_)->start();//participant join chat_room, server read header
     //std::move : no element will be copied, socket will be moved in the shared pointer
    }
    do_accept();
   });
 }

private:
 boost::asio::ip::tcp::acceptor acceptor_;
 chat_room room_;
};


int main(int argc, char* argv[]) {

 try {
  if (argc < 2) {
     std::cerr << "Select chat_server port <port> [<port> ... ] that host services";
     std::cerr << "Server can handle multiple chat room, one for each port.";
     return 1;
  }
  boost::asio::io_context io_context;

  std::list<chat_server> servers;
  //Can open multiple chat room with different port number
  for (int i = 1; i < argc; ++i) {
   boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v6(), std::atoi(argv[i]));
   //emplace_back avoids the extra copy or move operation required when using push_back. (C++ 11).
   servers.emplace_back(io_context, endpoint);
  }

  io_context.run();
 }
 catch (std::exception& e) {
   std::cerr << "Exception: " << e.what() << "\n";
 }
 return 0;

}
