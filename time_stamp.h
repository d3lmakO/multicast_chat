//time_stamp.h
//~~~~~~~~~~~~
//
//Author: Alberto Geroldi (alberto.geroldi1@studenti.unimi.it)
//
//Description: class that return a string with local time used by multicast_sender
//and chat_client to send local time and data.

#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <ctime>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>

//exactly the size necessary to contain max length data and
//two square parenthesis.
const int time_size = 27;
class timeStamp {
 public:
  //ctor
  timeStamp() {}
  //dtor
  ~timeStamp() {}
  
  std::string get_time_stamp() {
   time_t t = time(0); //get time now 
   struct tm * timeinfo = localtime(&t); //portable local time 
   std::string str_time (asctime(timeinfo));
   char flat_time[60] {};
   str_time.copy(flat_time, strcspn(str_time.c_str(), "\r\n"));
   std::stringstream ss;
   ss << '[' << flat_time << ']';
   std::string source = ss.str();
   std::vector<char> time( source.begin(), source.end() );
   time.resize(time_size);
   char * c_time = &(time[0]);
   return c_time;
  };  
};


#endif //TIME_STAMP_H
