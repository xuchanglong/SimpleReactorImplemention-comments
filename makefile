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

$(TARGET_SERVER):reactor_server_test.cc $(DIR_OBJ)/event_demultiplexer.o $(DIR_OBJ)/reactor.o $(DIR_OBJ)/global.o 
	$(CC) $^ -o $@ $(CFLAGS)

$(DIR_OBJ)/event_demultiplexer.o:$(DIR_EVENTDEMULTIPLEXER)/event_demultiplexer.cc $(DIR_EVENTDEMULTIPLEXER)/event_demultiplexer.h
	$(CC) -c $< -o $@ $(CFLAGS)

$(DIR_OBJ)/reactor.o:$(DIR_REACTOR)/reactor.cc $(DIR_REACTOR)/reactor.h $(DIR_OTHER)/timeheap.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

$(DIR_OBJ)/global.o:$(DIR_OTHER)/global.cc $(DIR_OTHER)/global.h $(DIR_OTHER)/singleton.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY:clean
clean:
	rm -rf $(DIR_OTHER)/*.o
	rm -rf $(TARGET_SERVER)
	rm -rf $(TARGET_CLIENT)