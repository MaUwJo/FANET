# FANET setting

#CC=g++
#CFLAGS=-c -Wall `mysql_config --cflags` `mysql_config --libs`
#CFLAGS=-c -Wall -I /usr/include/mariadb/server
CFLAGS=-c -Wall

CC=gcc
#CFLAGS=-c -Wall -o `mysql_config --libs` `mysql_config --cflags`
LIBS=-lwiringPi -lmariadb -lm

DEPS= *.h

OBJS= fanet_routing.o \
        fanet_t0_ack.o\
        fanet_t1_tracking.o\
        fanet_t2_name.o \
        fanet_t3_messenger.o \
        fanet_t4_service.o \
        fanet_t7_tracking.o\
        fanet_global.o \
        fanet_mac.o \
        fanet_mysql.o \
        fanet_radio.o \
        fanet_terminal.o 

TARGETS= fanet_station msg_sender

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS) 

all: $(TARGETS)

fanet_station: 	$(OBJS) fanet_station.o
	$(CC) $(OBJS) fanet_station.o $(LIBS) -o fanet_station

msg_sender: $(OBJS) msg_sender.o
	$(CC) $(OBJS) msg_sender.o $(LIBS) -o msg_sender

clean:
	rm *.o $(TARGETS)


