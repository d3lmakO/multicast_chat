//File: multicast_sender.cc
//~~~~~~~~~~~~~~~~~~~~~
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)
//
//Description:
//Base class for sending data via IPV6 multicast

#include "multicast_sender.h"
#include "time_stamp.h"
#include "async_getline.h"
#include "multicast_receiver.h"
#include "setNickname.h"

Sender::Sender(const std::array<char, 20>& nickname,
 boost::asio::io_context& io_context,
 const boost::asio::ip::address& multicast_address)
 : endpoint_(multicast_address, multicast_port_sender),
   socket_(io_context, endpoint_.protocol()),
   timer_(io_context),
   message_(" ") {
 setNickname Nn;
 nickname_ = Nn.NickName(nickname);
 socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
 socket_.set_option(boost::asio::ip::multicast::enable_loopback(true));
 socket_.set_option(boost::asio::ip::multicast::hops(1));
 socket_.set_option(boost::asio::ip::multicast::join_group(multicast_address));
 do_send();
};

Sender::~Sender() {};

void Sender::do_send() {
 std::vector<int> nBgetline = non_blocking_getline();
 std::vector<char> itcConversion = iTc_converter(nBgetline);
 message_ = cTstr(itcConversion);
 std::string mess_sent = nickname_ + " " + ":" + " " + message_ + "%$%$";
 socket_.async_send_to(
  boost::asio::buffer(mess_sent), endpoint_,
  //boost::system::error_code saves an error code in a variable of type int.
  [this](boost::system::error_code ec, std::size_t bytes) {
   if (!ec && message_ != close_connection) {
    do_timeout();
   }
   else if (!ec && message_ == close_connection) {
    do_timeout();
    socket_.close();
    std::cout << "Multicast sender connection close." << std::endl;
   }
  });
};

void Sender::do_timeout() {
 timer_.expires_after(std::chrono::seconds(1));
 timer_.async_wait(
 [this](boost::system::error_code ec) {
  if (!ec) {
   do_send();
   std::cout << "\n";
  } 
 });
};

//convertion from int to char
std::vector<char> Sender::iTc_converter(std::vector<int> vec)
{
 std::vector<char> conv_string;
 for (unsigned i = 0; i < vec.size(); ++i) 
 {
  char c = vec[i]; 
  conv_string.push_back(c);
 }
 return conv_string;
}

//convertion from vector to string
std::string Sender::cTstr(std::vector<char> c_vec)
{
 std::string pass_s;
 for (unsigned j = 0; j < c_vec.size(); ++j)
 {
  //insert one char at time in position j
  pass_s.insert(j,1,c_vec[j]);
 }
 return pass_s;
}



