/*
 * msg_sender.c
 *
 * Copyright 2019  <pi@RPi4>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>

#include "fanet_struct.h"
#include "fanet_radio.h"
#include "fanet_mac.h"
#include "fanet_mysql.h"
#include "fanet_terminal.h"
#include "fanet_t0_ack.h"
//#include "fanet_t1_tracking.h"
//#include "fanet_t2_name.h"
#include "fanet_t2_name.h"
#include "fanet_t3_messenger.h"
//#include "fanet_t4_service.h"
//#include "fanet_routing.h"

#include <sys/ioctl.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

time_t t;
struct tm *tm;


struct timeval tv;
struct tm* ptm;
char time_string[40];
long milliseconds;

byte sf;

enum { SF7=7, SF8, SF9, SF10, SF11, SF12 };

// Set spreading factor (SF7 - SF12)
byte sf = SF7;


void die(const char *s)
{
    perror(s);
    exit(1);
}

int main (int argc, char *argv[])
{

	sql_login();

    init_fanet_radio(1000, false);

    if (argc > 0) {
        printf ("Message: %s \n", argv[1]);
        send_mess(argv[1]);
    }

    return (0);
}
