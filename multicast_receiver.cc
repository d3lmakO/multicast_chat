//File: multicast_receiver.cc
//~~~~~~~~~~~~~~~~~~~
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)
//
//Description:
//Base class for receiving data via IPV6 multicast

#include "multicast_receiver.h"
#include "multicast_sender.h"
#include "time_stamp.h"
#include "setNickname.h"

Receiver::Receiver(const std::array<char, 20>& nickname,
 boost::asio::io_context& io_context,
 const boost::asio::ip::address& listen_address, 
 const boost::asio::ip::address& multicast_address)
 : receiver_socket_(io_context) {
 setNickname Nn;
 nickname_ = Nn.NickName(nickname);
 // Create the socket so that multiple may be bound to the same address.
 boost::asio::ip::udp::endpoint listen_endpoint(listen_address,multicast_port);
 receiver_socket_.open(listen_endpoint.protocol()); 
 //socket option to allow the socket to be bound to an address that is already in use.
 receiver_socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
 //Socket option determining whether outgoing multicast packets 
 //will be received on the same socket if it is a member of the multicast group. 
 receiver_socket_.set_option(boost::asio::ip::multicast::enable_loopback(true));
 //Socket option for time-to-live associated with outgoing multicast packets. 
 //basically represents different layers of the network
 // default value of 1, to prevent them to be forwarded beyond the local network.(values from 0 to 255)
 receiver_socket_.set_option(boost::asio::ip::multicast::hops(4));
 //bind endpoint tell the program to use this local UDP port for receiving data.
 //bind socket with a particular IP address and a protocol port number.
 receiver_socket_.bind(listen_endpoint);
 //for a process to receive multicast datagrams it has to ask the kernel to join the group and bind the port those datagrams were being sent to. 
 //The UDP layer uses both the destination address and port to demultiplex the packets and decide which socket(s) deliver them to
 //it is necessary to advise the kernel which multicast groups we are interested in. That is, we have to ask the kernel to "join" those multicast groups
 receiver_socket_.set_option(boost::asio::ip::multicast::join_group(multicast_address)); 
 do_receive();
};

Receiver::~Receiver() {};

void Receiver::do_receive() {
 receiver_socket_.async_receive_from(
 boost::asio::buffer(data_), receiver_endpoint_,
 [this](boost::system::error_code ec, std::size_t length ) {
  //regExprStr try to match all posible words to find "quit" in buffer
  //so we can close receiver_socket_
  std::string buf_read = parsing_data_(data_);

  std::string val_str = ctr_emptyStr_(buf_read);
  std::regex rgx{"([\\w]+)\\s\\:\\s([\\w]+)"};
  std::smatch match;
  std::regex_search(buf_read, match, rgx);
  std::string name = match[1];
  std::string escape_value = match[2];
  //timeStamp: class that stamp current time.
  timeStamp ts;
  std::string out_buff = ts.get_time_stamp() + buf_read + "\n";
  //this loop open dev/null file and redirects there output if empty string is recived,
  //so we can se at screen only non empty string.
  if (val_str.empty()) {
   //redirect empty string in dev/null
   std::ofstream file("/dev/null");  
   std::streambuf* strm_buffer = std::cout.rdbuf();
   std::cout.rdbuf(file.rdbuf());
   std::cout.write(out_buff.data(), out_buff.length());
   std::cout.rdbuf (strm_buffer);
   do_receive();
     } else if (!ec && name == nickname_ && escape_value == close_connection) {
     std::cout.write(out_buff.data(), out_buff.length());
     std::chrono::seconds(1);
     receiver_socket_.close();
     std::cout << "MUlticast receiver connection close" << std::endl;
  }
  else {
   std::cout.write(out_buff.data(), out_buff.length());
   do_receive();
  }
 });
};

//search the key string and print substring from 0 to key string 
std::string Receiver::parsing_data_(std::array<char,1024> arr) {
  std::ostringstream os;
 os << arr.data();
 std::string buf_r = os.str();
 std::string::size_type key_str = buf_r.find("%$%$");
 std::string ret_string = buf_r.substr(0, key_str);
 os.str("");
 return ret_string;
}

//search if the string is empty 
std::string Receiver::ctr_emptyStr_(std::string eval_str) {
 std::string::size_type k_s = eval_str.find(":"); 
 std::string find_str = eval_str.substr(k_s + 2 , eval_str.length());
 return find_str;
}
