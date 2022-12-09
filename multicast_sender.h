//File: multicast_sender.h
//~~~~~~~~~~~~~~~~~~~
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)

#ifndef MULTICAST_SENDER_H
#define MULTICAST_SENDER_H

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>

constexpr short multicast_port_sender = 30001;
const std::string close_connection = "quit";

class Sender {
public:
 Sender(const std::array<char, 20>& nickname,
  boost::asio::io_context& io_context,
  const boost::asio::ip::address& multicast_address);
 ~Sender();
private:
 void do_send();
 void do_timeout();
 std::vector<char> iTc_converter(std::vector<int>);
 std::string cTstr(std::vector<char>);
 boost::asio::ip::udp::endpoint endpoint_;
 boost::asio::ip::udp::endpoint remote_endpoint_;
 boost::asio::ip::udp::socket socket_;
 boost::asio::steady_timer timer_;
 std::string message_;
 std::string nickname_;
};


#endif // MULTICAST_SENDER_H

