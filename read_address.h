//read_address.h
//~~~~~~~~~~~~~~~~~
//
//Author: Alberto Geroldi
// 
#ifndef READ_ADDRESS_H
#define READ_ADDRESS_H

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

//read multicast address ipv6 and listen adress ipv6 and pass them
//to boost::asio::ip::make_address
class ReadAddress {
 public:  
 //ctor
 ReadAddress() {}
 //dtor
 ~ReadAddress() {}

 char* multi_address(std::string multicast_address) {
  char* m_address = &*multicast_address.begin();
  return m_address;
 };

 char* list_address(std::string listen_address) {
  char* l_address = &*listen_address.begin();
  return l_address;
 };
 
 private:

};

#endif //READ_ADDRESS_H
