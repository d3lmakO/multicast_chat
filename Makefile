CXXFLAGS=-g -pthread -Wall -std=c++17
LDLIBS=-lboost_system -lboost_thread -lpthread

chat: main.o multicast_sender.o multicast_receiver.o chat_client.o
	$(LINK.cc) $^ -o $@

main.o: main.cc 
	$(COMPILE.cc) main.cc -o main.o 

multicast_sender.o: multicast_sender.cc multicast_sender.h 
	$(COMPILE.cc) multicast_sender.cc -o multicast_sender.o

multicast_receiver.o: multicast_receiver.cc  
	$(COMPILE.cc) multicast_receiver.cc -o multicast_receiver.o

chat_client.o: chat_client.cc chat_client.h
	$(COMPILE.cc) chat_client.cc -o chat_client.o

.PHONY: clean

clean: 
	$(RM) *.o
