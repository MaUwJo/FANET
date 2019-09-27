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
//#include "fanet_mysql.h"
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

/*
void receivepacket()
{
	sFanetMAC	  _fanet_mac;
	sRawMessage   _rx_radio;
	sRawMessage	  _rx_payload;
	sRadioData 	  _radiodata;

	if(read_rx_data(&_rx_radio, &_radiodata))
    {
		_fanet_mac.valid_bit = 1;

		if (_radiodata.crc_err)
			terminal_message_crc_err (0,0,&_radiodata, &_fanet_mac);

		fanet_mac_decoder (&_fanet_mac, &_rx_radio, &_rx_payload);
		if (!_fanet_mac.valid_bit)
			terminal_message_mac_err (0,0,&_radiodata, &_fanet_mac);

		terminal_message_raw (0,0, &_radiodata, &_fanet_mac, &_rx_radio);


		if (_fanet_mac.valid_bit && !_radiodata.crc_err)
		{
			switch (_fanet_mac.type)
			{
//				case 0:	type_0_ack_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
//				case 1:	type_1_tracking_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 2:	type_2_name_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 3: type_3_message_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
//				case 4: type_4_service_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				//default: terminal_mac_info (0,0,&_fanet_mac); printf ("\n");
			}
			if (_fanet_mac.ack)
				send_ack (&_fanet_mac);
		}
		else
		{
//			terminal_rf_info (0,0,&_radiodata, &_fanet_mac);
//			terminal_mac_info (0,0,&_radiodata, &_fanet_mac); printf ("\n");
		}

//TTT		update_routing (&_radiodata, &_fanet_mac);
    }
}
*/

int main (int argc, char *argv[])
{

    init_fanet_radio();

    //delay(1000);

    if (argc > 0) {
        printf ("Message: %s \n", argv[1]);
        send_mess(argv[1]);
    }

    return (0);
}
