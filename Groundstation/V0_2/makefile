# FANET setting

#CC=g++
#CFLAGS=-c -Wall `mysql_config --cflags` `mysql_config --libs`
CFLAGS=-c -Wall -I /usr/include/mariadb/server 

CC=gcc
#CFLAGS=-c -Wall -o `mysql_config --libs` `mysql_config --cflags`
LIBS=-lwiringPi -lmariadb -lm

all: fanet_station msg_sender

fanet_station: main.o fanet_routing.o fanet_t0_ack.o fanet_t1_tracking.o fanet_t2_name.o fanet_t3_messenger.o fanet_t4_service.o fanet_global.o fanet_mac.o fanet_mysql.o fanet_radio.o fanet_terminal.o
	$(CC) main.o fanet_routing.o fanet_t0_ack.o fanet_t1_tracking.o fanet_t2_name.o fanet_t3_messenger.o fanet_t4_service.o fanet_global.o fanet_mac.o fanet_mysql.o fanet_radio.o fanet_terminal.o $(LIBS) -o fanet_station

main.o: main.c
	$(CC) $(CFLAGS) main.c

fanet_t2_name.o: fanet_t2_name.c
	$(CC) $(CFLAGS) fanet_t2_name.c

fanet_t0_ack.o: fanet_t0_ack.c
	$(CC) $(CFLAGS) fanet_t0_ack.c

fanet_t1_tracking.o: fanet_t1_tracking.c
	$(CC) $(CFLAGS) fanet_t1_tracking.c

fanet_t4_service.o: fanet_t4_service.c fanet_t4_service.h
	$(CC) $(CFLAGS) fanet_t4_service.c

fanet_t3_messenger.o: fanet_t3_messenger.c
	$(CC) $(CFLAGS) fanet_t3_messenger.c

msg_sender: msg_sender.o fanet_global.o fanet_mac.o fanet_mysql.o fanet_t3_messenger.o fanet_radio.o fanet_terminal.o
	$(CC) msg_sender.o fanet_global.o fanet_t3_messenger.o fanet_mac.o fanet_mysql.o fanet_radio.o fanet_terminal.o $(LIBS) -o msg_sender

msg_sender.o: msg_sender.c
	$(CC) $(CFLAGS) msg_sender.c

fanet_mac.o: fanet_mac.c
	$(CC) $(CFLAGS) fanet_mac.c

fanet_global.o: fanet_global.c
	$(CC) $(CFLAGS) fanet_global.c

fanet_routing.o: fanet_routing.c
	$(CC) $(CFLAGS) fanet_routing.c

fanet_radio.o: fanet_radio.c fanet_radio.h
	$(CC) $(CFLAGS) fanet_radio.c

fanet_terminal.o: fanet_terminal.c
fanet_terminal.o: fanet_terminal.c
	$(CC) $(CFLAGS) fanet_terminal.c

	$(CC) $(CFLAGS) fanet_terminal.c

fanet_mysql.o: fanet_mysql.c
	$(CC) $(CFLAGS) fanet_mysql.c

clean:
	rm *.o fanet_station rm *.o fanet_mysql



