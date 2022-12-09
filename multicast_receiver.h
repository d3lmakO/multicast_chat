//File: multicast_receiver.h
//~~~~~~~~~~~~~~~~~~~
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)

#ifndef MULTICAST_RECEIVER_H
#define MULTICAST_RECEIVER_H

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <regex>
#include <boost/asio.hpp>

constexpr short multicast_port = 30001;

class Receiver {
 public:
  Receiver(const std::array<char, 20>& nickname,
   boost::asio::io_context& io_context,
   const boost::asio::ip::address& listen_address,
   const boost::asio::ip::address& multicast_address);
  ~Receiver();
 private:
  void do_receive();
  std::string parsing_data_(std::array<char,1024>);
  std::string ctr_emptyStr_(std::string);
  boost::asio::ip::udp::socket receiver_socket_;
  boost::asio::ip::udp::endpoint receiver_endpoint_;
  std::array<char,1024> data_;
  std::string nickname_;
};

#endif //MULTICAST_RECEIVER_H



