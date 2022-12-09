//message.h
//~~~~~~~~~~~~~~~~
//
//
//#include guards:  it checks if a unique value is defined. Then if it's not defined, it defines it and continues to the rest of the page.
//That prevents double declaration of any identifiers such as types, enums and static variables.
#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>

class Chat_message {
public:
 enum { header_length = 4 };
 enum { max_body_length = 512 };
  
 Chat_message() : body_length_(0) 
 {
 }

 const char* data() const {
  return data_;
 }

 char* data() {
  return data_;
 }
 //header_length:  In the header you put information about the length and probably
 //about the type of the next message. Then you send the header in front of the message body.
 //
 //std::size_t is the unsigned integer type of the result of the sizeof operator
 //std::size_t can store the maximum size of a theoretically possible object of any type
 std::size_t length() const {
  return header_length + body_length_;
 }
  
 const char* body() const {
  return data_ + header_length;
 }

 char* body() {
  return data_ + header_length;
 }
 //body_length can change, length of body si different between a message and another one 
 std::size_t body_length() const {
  return body_length_;
 }

 void body_length(std::size_t new_length) {
  body_length_ = new_length;
   
  if (body_length_ > max_body_length) {
   body_length_ = max_body_length;
  }
 }
 //char *strncat( char *dest, const char *src, std::size_t count ):
 //Appends a byte string pointed to by src to a byte string pointed to by dest. 
 //At most count characters are copied. The resulting byte string is null-terminated. 
 //std::atoi(const char *str): Interprets an integer value in a byte string pointed to by str. 
 //Discards any whitespace characters until the first non-whitespace character is found, 
 //then takes as many characters as possible to form a valid integer number representation and converts them to an integer value.
 bool decode_header() {
  char header[header_length + 1] = "";
  std::strncat(header, data_, header_length);
  body_length_ = std::atoi(header);

  if (body_length_ > max_body_length) { 
   body_length_ = 0;
   return false;
  }
  else return true;
 }
 //std::sprintf: same of printf except that output is sent to buffer; opposite to std::atoi(char);
 //int sprintf ( char * str, const char * format, ... )
 //Composes a string with the same text that would be printed if format was used on printf, 
 //but instead of being printed, the content is stored as a C string in the buffer pointed by str.
 void encode_header() {
  char header[header_length + 1] = "";
  std::sprintf(header,"%4d", static_cast<int>(body_length_));
  //std::memmove: Copies count characters from the object pointed to by header to the object pointed to by data_. 
  //Both objects are reinterpreted as arrays of unsigned char 
  std::memmove(data_, header, header_length);
 }

private:
 std::size_t body_length_;
 char data_[header_length + max_body_length];
  
};

#endif // CHAT_MESSAGE_H
