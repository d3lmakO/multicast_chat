//File: main.cc
//~~~~~~~~~~~~
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)
//
//Description: multicast chat ----- client-server class. You can choose standard client-server class
//when multicast chat is over. Thinked as a "normal" mode to connect after multicast chat failed, 
//because no one is listening.

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "read_address.h"
#include "time_stamp.h"
#include "multicast_sender.h"
#include "multicast_receiver.h"
#include "message.h"
#include "chat_client.h"

int main(int argc, char* argv[]) {

////////////////////////////////////////////////////////////////////////////
//MULTICAST CHAT
/////////////////////////////////////////////////////////////////////////////

 std::cout << "Welcome to multicast/client-server chat" << std::endl;
 std::cout << "\n";
 std::array<char, 20> nickname; 
 try {
  
  if (argc != 4) {
   std::cerr << "Please follow the instructions to run it! " << std::endl;
   std::cerr << "1) Write your Nickname" << std::endl;
   std::cerr << "2) Insert listen address: ipv6 --> 0::0 " << std::endl;
   std::cerr << "3) Insert multicast address:: ipv6 --> ff31::8000:1234 " << std::endl;
   std::cerr << "4) Press <enter>" << std::endl;
   std::cerr << "5) Insert 'quit' to close Multicast connection" << std::endl;
   std::cerr << "----------------------------------------------" << std::endl;
   return 1;
  }
  
   strcpy(nickname.data(), argv[1]);
   ReadAddress ra;
   boost::asio::io_context io_context; 
   Receiver r(nickname,io_context, boost::asio::ip::make_address(ra.list_address(argv[2])), 
    boost::asio::ip::make_address(ra.multi_address(argv[3])));
   Sender s(nickname, io_context, boost::asio::ip::make_address(ra.multi_address(argv[3])));
   std::thread t([&io_context] { io_context.run(); });
   t.join();
 }
 catch (std::exception& e) {
  std::cerr << "Exception :" << e.what() << std::endl;
 }
////////////////////////////////////////////////////////////////////////////
//END MULTICAST CHAT
////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//CLIENT-SERVER CHAT
///////////////////////////////////////////////////////////////////////////
 try {
      std::cout << "Do you want to run a classic client - server class or you just want to exit?" << std::endl;
      std::cout << "Please enter <Y/n>: Y-> run; n -> exit" << std::endl; 
      std::cout << '\n';
      char enter_exit_command;
      std::string host;
      std::string port;
      std::cin >> enter_exit_command;
      std::cout << '\n';
      if (enter_exit_command == 'Y') {
       std::cout << "Insert <host>. For exemple -> ipv6 localhost is ::1, ipv4 localhost is 127.0.0.1" << std::endl;
       std::cout << '\n';
       std::cout << "->";
       while (std::getline(std::cin, host)) {
        if(!host.empty()) {break;}
       }
       std::cout << '\n';
       std::cout << "Select one <port>. Server is running on port 45000, 45054" << std::endl;
       std::cout << '\n';
       std::cout << "->";
       while (std::getline(std::cin, port)) {
        if (!port.empty()) {break;}
       }
       std::cout << '\n';
       std::cout << "Connection to service on server:" << std::endl;
       std::cout << "HOST:" << " " << host << " " << " " << " " << "PORT:" << " " << port << std::endl;
       std::cout << '\n';
       boost::asio::io_context io_context;
       boost::asio::ip::tcp::resolver resolver(io_context);
       //Name: identify person in a chat
       std::string name = argv[1];
       //mess: message sent by a chat participant
       std::string mess;   
       //connect to server: in this case host is on the same pc, so we can insert the localhost ::1 or 127.0.0.1
       //in general host could be external to personal pc
       auto endpoints = resolver.resolve(host, port);
       Chat_client c(io_context, endpoints);
       std::thread t([&io_context](){ io_context.run(); });
       Chat_message msg;
       //begin loop of client-server chat
       while(std::getline(std::cin, mess)) { 
       //close_connection is in multicast_receiver.h: is the same command
        if ( mess != close_connection) { 
         timeStamp ts;
         char line[Chat_message::max_body_length + 1];
         std::string complete = ts.get_time_stamp() + name + ":" + " " + mess;
         strcpy(line, complete.c_str());
         msg.body_length(std::strlen(line));
         std::memcpy(msg.body(), line, msg.body_length());
         msg.encode_header();
         c.write(msg);
        } else {
          break;
        }
       }

       //close handled by asio::post
       c.close();

       t.join();

      } else {
         std::cout << "Chat: closed." << std::endl;
      }
 } 
 catch (std::exception& e) {
  std::cerr << "Exception :" << e.what() << std::endl;
 }

//////////////////////////////////////////////////////////////////////
//END CLIENT-SERVER CHAT
//////////////////////////////////////////////////////////////////////

 return 0;
};
