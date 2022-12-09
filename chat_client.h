//chat_client.h
//~~~~~~~~~~~~~~
//
//Alberto Geroldi - 2019
//
#ifndef CHAT_CLIENT_H 
#define CHAT_CLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

typedef std::deque<Chat_message> chat_message_queue;

class Chat_client {
 public:
 Chat_client(boost::asio::io_context& io_context,
  const boost::asio::ip::tcp::resolver::results_type& endpoints);
 
  void write(const Chat_message& msg);
  void close();
 
 private:
  std::string complete_message(std::string, std::string);
  void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints);
  void do_read_header();
  void do_read_body();
  void do_write();
  boost::asio::io_context& io_context_;
  boost::asio::ip::tcp::socket socket_;
  Chat_message read_msg_;
  chat_message_queue write_messages_;
};

#endif // CHAT_CLIENT_H
