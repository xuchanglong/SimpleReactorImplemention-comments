CC = g++
CFLAGS = -g -Wall

TARGET_SERVER = server.o
TARGET_CLIENT = client.o

DIR_OBJ = ./obj
DIR_EVENTDEMULTIPLEXER = ./eventDemultiplexer
DIR_EVENTHANDLER = ./eventhandler
DIR_OTHER = ./other
DIR_REACTOR = ./reactor

all:$(TARGET_SERVER) $(TARGET_CLIENT)

#$(TARGET_SERVER):reactor_server_test.cc $(DIR_EVENTDEMULTIPLEXER)/event_demultiplexer.cc $(DIR_REACTOR)/reactor.cc $(DIR_OTHER)/global.cc
#	$(CC) $< -o $@ $(CFLAGS)

#$(TARGET_CLIENT):reactor_client_test.cc $(DIR_EVENTDEMULTIPLEXER)/event_demultiplexer.cc $(DIR_REACTOR)/reactor.cc $(DIR_OTHER)/global.cc
#	$(CC) $< -o $@ $(CFLAGS)
g++ -o $(TARGET_SERVER) reactor_server_test.cc ./eventDemultiplexer/event_demultiplexer.cc ./reactor/reactor.cc ./other/global.cc

g++ -o $(TARGET_CLIENT) reactor_client_test.cc ./eventDemultiplexer/event_demultiplexer.cc ./reactor/reactor.cc ./other/global.cc

.PHONY:clean
clean:
	rm -rf $(TARGET_SERVER)
	rm -rf $(TARGET_CLIENT)