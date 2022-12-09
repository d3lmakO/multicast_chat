// setNickname.h
// ~~~~~~~~~~~~~~~~~~
// Alberto Geroldi - 2019
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)
//
//Description: class that returns a string. Used by multicast sender to send chat nickname.

#ifndef SETNICKNAME_H 
#define SETNICKNAME_H

#include <array>
#include <iostream>
#include <cstring>

//convert std::array with nickname in std::string and pass it
//to multicast_sender
class setNickname {
 public:
  //ctor 
  setNickname() {}
  //dtor
  ~setNickname() {}

  std::string NickName(const std::array<char, 20>& nickname) {
   strcpy(nickname_.data(), nickname.data());
   char* string_name = &*nickname_.begin();
   return string_name;
  };

 private:
  std::array<char, 20> nickname_;

};

#endif //SETNICKNAME_H
