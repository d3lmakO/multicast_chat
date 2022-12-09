//async_getline.h
//~~~~~~~~~~~~~~~~~
//Author: Alberto Geroldi
//
//Description:
//Non blocking terminal for asynchronous operations, change vector to string, read until escape characters.

#ifndef ASYNC_GETLINE_H
#define ASYNC_GETLINE_H

#include <iostream>
#include <vector>
#include <unistd.h> //remove canonical mode and in general get and set terminal attributes
#include <termios.h>
#include <string.h> //bcopy()
#include <sys/time.h>//termios, fd_set

#define KEY_TIMEOUT 5

//struct that contains different memebers like input modes, output modes,..
struct termios old_term;

std::vector<int> non_blocking_getline();
void set_term_();
void reset_term_();
int input_char();
int key_press();

//set raw terminal
void set_term_()
{
 //fd file descriptor 0 stdin 1 stdout 2 stderr
 int fd;

 struct termios mio_term;

 fd = fileno(stdin);
 //tcgetattr: get the parameters associated with the terminal referred to by fd 
 //and store them in the termios structure. The fd argument is an open file descriptor associated with a terminal.
 tcgetattr(fd, &mio_term);
 //copy n bytes from mio_term to old_term
 bcopy(&mio_term,&old_term,sizeof(mio_term));
 atexit(reset_term_);
 //cfmakeraw: terminal in raw mode,  input is available character by character, 
 //echoing is disabled, and all special processing of terminal input and output characters is disabled.
 cfmakeraw(&mio_term);
 //tcsetattr: set the parameters associated with the terminal referred to by fd
 //TCSANOW -> the change occurs immediately
 tcsetattr(fd, TCSANOW, &mio_term);
}

//reset canonical terminal
void reset_term_()
{
 int fd;
 fd = fileno(stdin);
 tcsetattr(fd, TCSANOW, &old_term);
}

//return char in his int value
int input_char()
{
 int r;
 unsigned char c;
 //read() : read up to sizeof(c) bytes from file descriptor (in this case stdin) into the buffer.
 //Read one char per time.
 //On success, the number of bytes read is returned
 //and the file position is advanced by this number.
 if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
 } else {
    return c;
 }
}

int set_fflush_()
{
 //fflush(): clear the output buffer and move the buffered data to console
 return(fflush(stdout));
}

int key_press()
{
 //returns time interval in seconds and nanoseconds
 struct timeval timeout;
 //fd_set: fixed size buffer
 fd_set input;
 //seconds
 timeout.tv_sec = KEY_TIMEOUT;
 //microseconds
 timeout.tv_usec = 0;
 FD_SET(fileno(stdin), &input);
 //select: allow a program to monitor multiple file descriptors, waiting until one or more 
 //fd become ready for I/O operations
 //timeout indicate how much time was left
 return select(1, &input, NULL, NULL, &timeout);
}

//return a vector with the line sent via multicast_sender
std::vector<int> non_blocking_getline()
{
 int kvalue;
 //non blocking mode
 set_term_();
 
 std::vector<int> mystring;  
 //infinite loop which will run untill a break statement is issued explicitly
 while(1) {
  //control if select is a positive integer type
  if (key_press() <= 0)
  {
   break;
  } else if (key_press() > 0) {
     kvalue = input_char();
        //13 new line ascii char
     if (kvalue == 13) {
      printf("\n\r");
      break;
     } else if ( kvalue >= 20 && kvalue != 127) {
        mystring.push_back(kvalue);
        printf("%c", kvalue);
        set_fflush_();
        //127 cancel ascii char
     } else if ( kvalue == 127) {
        if (!mystring.empty())
        {
        mystring.pop_back(); 
        printf("\b");
        printf(" ");
        printf("\b");
        set_fflush_();
        } else {
           printf("\r");
        }
     }
    }
 }
 //canonical mode
 reset_term_();
 return mystring;
}

#endif //ASYNC_GETLINE_H
