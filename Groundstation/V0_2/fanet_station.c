/*
 * main.c
 *
 * Copyright 2019  <pi@RPi3B_FANET_2>
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
#include "fanet_db.h"
#include "fanet_terminal.h"
#include "fanet_t0_ack.h"
#include "fanet_t1_tracking.h"
#include "fanet_t2_name.h"
#include "fanet_t3_messenger.h"
#include "fanet_t4_service.h"
#include "fanet_t7_tracking.h"
#include "fanet_routing.h"

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


// Configure the Weather Stations (In a next Version: Better solution to read out a SQL databench)
const char * stations_id [] = {
        "4217", // GER Dummy
        "H711",    // Holfuy Hohenberg
        "H795"     // Holfuy Orensberg
};
#define N_STATIONS (sizeof (stations_id) / sizeof (const char *))

const byte source_manufacturer_id = 0xFC;
const uint16_t source_unique_id [] = {0x9017,0x9711,0x9795};


#define	START_OFFSET_DATA	4;				// Starts the first transmission of weather data after 4 sec. when the software has started
#define START_OFFSET_NAME	8;				// Starts the first transmission of weather station name after 8 sec. when the software has started
#define INTERVAL_DATA		10;				// Sends the next weather data in a 10 sec interval
#define INTERVAL_NAME		30;				// Sends the next weather station name in a 30 sec interval

#define MAX_OLD_WEATHER_DATA	1800;		// 1800 seconds = 30 min -> If weather data are older, they will not sended.

boolean b_send_gti = false;                 // send groundtracking info for ground and weather stations

void die(const char *s)
{
    perror(s);
    exit(1);
}

extern unsigned int  freq; // = 868200000; // in Hz! (868.2)  - TTTTT

void system_data ()
{
	static sSystem	_system_data;
	sRadioData		_radiodata;

	static int 		_yy;
	static int		_rssi_sum_1min;
	static float	_rssi_sum_15min;
	static int		_rssi_max_1min = -157;
	static int		_rssi_max_15min = -157;
	static int  	_rssi_avg_1min_counter;
	static int  	_rssi_avg_15min_counter;

	if (_yy == 500)			// 500 ms update rate
	{
		t = time(NULL);
		tm = localtime(&t);

		gettimeofday (&tv, NULL);
		ptm = localtime (&tv.tv_sec);
		milliseconds = tv.tv_usec / 1000;
		strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);

		read_radio_data(&_radiodata);

		if ((_radiodata.rssi > -130) & (_radiodata.rssi < 0))		// Ignore RSSI values <-130dBm and > 0dBm
		{
			_rssi_sum_1min += _radiodata.rssi;
			_rssi_avg_1min_counter++;
			if (_radiodata.rssi > _rssi_max_1min)
				_rssi_max_1min = _radiodata.rssi;
		}

        printf("|%s.%03ld| RSSI:%+4ddBm  RX Headers: %d \b\r", time_string, milliseconds, _radiodata.rssi, _radiodata.rx_headers);
		fflush(stdout);

		if (!(t % 60) && _rssi_avg_1min_counter > 2)		// Modulo Divison: Volle Minute = kein Rest bei Disision durch 60 (1 min)
		{
			radio_info(&_system_data);						// Reads the tx/rx packets and tx time. After reading, values will be reset to 0.

			_system_data.timestamp_1min = t;
			_system_data.rssi_avg_1min = (float)_rssi_sum_1min / (float)_rssi_avg_1min_counter;
			_system_data.rssi_max_1min = _rssi_max_1min;

			printf ("\nSystem  1 min Data: %ld RSSI avg: %+7.2fdBm  RSSI max: %ddBm  RX packets: %d  TX packets: %d  TX time: %dms\n",
				_system_data.timestamp_1min,
				_system_data.rssi_avg_1min,
				_system_data.rssi_max_1min,
				_system_data.rx_packets_1min,
				_system_data.tx_packets_1min,
				_system_data.tx_time_1min);

			write_system_data_1min (&_system_data);
			//delete_old_data();

			_rssi_sum_15min += _system_data.rssi_avg_1min;
			_rssi_avg_15min_counter++;
			if (_rssi_max_1min > _rssi_max_15min)
				_rssi_max_15min = _rssi_max_1min;

			_system_data.rx_packets_15min += _system_data.rx_packets_1min;
			_system_data.tx_packets_15min += _system_data.tx_packets_1min;
			_system_data.tx_time_15min += _system_data.tx_time_1min;

			_rssi_sum_1min = 0;
			_rssi_avg_1min_counter = 0;
			_rssi_max_1min = -157;

			_system_data.rx_packets_1min = 0;
			_system_data.tx_packets_1min = 0;
			_system_data.tx_time_1min = 0;

			if (!(t % 900) && _rssi_avg_15min_counter > 2)		// Modulo Divison: Volle Minute = kein Rest bei Disision durch 900 (15 min)
			{
				_system_data.timestamp_15min = t;

				_system_data.rssi_avg_15min = _rssi_sum_15min /(float) _rssi_avg_15min_counter;
				_system_data.rssi_max_15min = _rssi_max_15min;

				printf ("\nSystem 15 min Data: %ld RSSI avg: %+7.2fdBm  RSSI max: %ddBm  RX packets: %d  TX packets: %d  TX time: %dms\n",
					_system_data.timestamp_15min,
					_system_data.rssi_avg_15min,
					_system_data.rssi_max_15min,
					_system_data.rx_packets_15min,
					_system_data.tx_packets_15min,
					_system_data.tx_time_15min);

				write_system_data_15min (&_system_data);

				_rssi_sum_15min = 0;
				_rssi_avg_15min_counter = 0;
				_rssi_max_15min = -157;

				_system_data.rx_packets_15min = 0;
				_system_data.tx_packets_15min = 0;
				_system_data.tx_time_15min = 0;
			}
		}
		_yy = 0;
	}
	_yy++;
}


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
				case 0:	type_0_ack_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 1:	type_1_tracking_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 2:	type_2_name_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 3: type_3_message_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 4: type_4_service_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				case 7:	type_7_tracking_receiver(&_radiodata, &_fanet_mac, &_rx_payload); break;
				//default: terminal_mac_info (0,0,&_fanet_mac); printf ("\n");
			}
			if (_fanet_mac.ack)
				send_ack (&_fanet_mac);
		}
		else
		{
			terminal_rf_info (0,0,&_radiodata, &_fanet_mac);
			terminal_mac_info (0,0,&_radiodata, &_fanet_mac); printf ("\n");
		}

		update_routing (&_radiodata, &_fanet_mac);
    }
}

void type_7_ground_tracking_coder (sRawMessage *_tx_message, sGroundTracking *_tracking)
{
// assume coordinates already encoded in _tx_message
// ground
    _tx_message->message[6] = ((_tracking->ground_type & 0x0F) << 4);
    if (_tracking->tracking) {
        _tx_message->message[6] |= 0x01;
    }
    _tx_message->m_length += 1;
}

void type_4_weather_2_ground_tracking_coder (sRawMessage *_tx_message, sWeather *_weather_data)
{
    for (int i=0; i<255; i++) { _tx_message->message[i] = '\0'; };
    encode_abs_coordinates (_tx_message, _weather_data->latitude, _weather_data->longitude);
    sGroundTracking _tracking;
    _tracking.tracking = true;
    _tracking.ground_type = 7;  // Ground station
    type_7_ground_tracking_coder(_tx_message, &_tracking);
}


void fanet_service_scheduler (void)
{
    sRadioData			_radiodata;
    sFanetMAC			_fanet_mac;
    sFanetMAC			_fanet_mac_1;
    sFanetMAC			_fanet_mac_7;
    sWeather			_tx_weather_data;
    sWeather			_rx_weather_data;
    sRawMessage			_tx_message;
    sName				_tx_name;
    sName				_rx_name;
    static byte			_i_data = 0;
    static byte			_i_name = 0;
    static uint16_t		_data_timer = START_OFFSET_DATA;
    static uint16_t		_name_timer = START_OFFSET_NAME;

    _fanet_mac.e_header	= false;
    _fanet_mac.forward	= false;
    _fanet_mac.cast		= false;
    _fanet_mac.signature_bit = false;
    _fanet_mac.valid_bit = true;

    //----
    _fanet_mac_7.e_header	= false;
    _fanet_mac_7.forward	= false;
    _fanet_mac_7.cast		= false;
    _fanet_mac_7.signature_bit = false;
    _fanet_mac_7.valid_bit = true;

    _fanet_mac_1.e_header	= false;
    _fanet_mac_1.forward	= false;
    _fanet_mac_1.cast		= false;
    _fanet_mac_1.signature_bit = false;
    _fanet_mac_1.valid_bit = true;
    //----

    if (!_data_timer)
    {
        _tx_message.m_length = 0;
        _fanet_mac.type	= 4;
        _fanet_mac.s_manufactur_id	= source_manufacturer_id;
        _fanet_mac.s_unique_id		= source_unique_id[_i_data];

        strcpy(_tx_weather_data.id_station, stations_id[_i_data]);

        get_weather_station (stations_id[_i_data], &_tx_weather_data);
        get_weather_data (&_tx_weather_data);
        //printf("Longitude %f\n", _weather_data.longitude);
        type_4_service_coder (&_tx_message, &_tx_weather_data);

        fanet_mac_coder (&_radiodata, &_fanet_mac, &_tx_message);

        // Monitor task
        //terminal_message_raw (1,0, &_radiodata, &_fanet_mac, &_tx_message);
        type_4_service_decoder (&_tx_message, &_rx_weather_data);
        terminal_message_4 (true, false, &_radiodata, &_fanet_mac, &_rx_weather_data);

        if (b_send_gti == true) {   // Send GroundTrackingInfo for WeatherStation
            /*
            _fanet_mac_7.type	= 7;
            _fanet_mac_7.s_manufactur_id	= source_manufacturer_id;
            _fanet_mac_7.s_unique_id		= source_unique_id[_i_data];
            _tx_message.m_length = 0;
            type_4_weather_2_ground_tracking_coder(&_tx_message, &_tx_weather_data);
            fanet_mac_coder (&_radiodata, &_fanet_mac_7, &_tx_message);

            sGroundTracking	_rx_ground_tracking;
            type_7_tracking_decoder (&_tx_message, &_rx_ground_tracking);
            terminal_message_7 (0,0, &_radiodata, &_fanet_mac_7, &_rx_ground_tracking);
*/
            _fanet_mac_1.type	= 1;
            _fanet_mac_1.s_manufactur_id	= source_manufacturer_id;
            _fanet_mac_1.s_unique_id		= source_unique_id[_i_data];
            _tx_message.m_length = 0;

            sTRACKING _tx_tracking;
            _tx_tracking.aircraft_type = 5;
            strcpy(_tx_tracking.aircraft_type_char, "gsX\0");
            _tx_tracking.longitude = _tx_weather_data.longitude;
            _tx_tracking.latitude = _tx_weather_data.latitude;
            _tx_tracking.altitude = _tx_weather_data.altitude;
            _tx_tracking.heading = 333.0;
            _tx_tracking.speed = 42.17;
            _tx_tracking.climb = 4.1;
            _tx_tracking.turn_rate = 3.2;
            type_1_tracking_coder (&_tx_message, &_tx_tracking);
            fanet_mac_coder (&_radiodata, &_fanet_mac_1, &_tx_message);

            sTRACKING _rx_tracking;
            type_1_tracking_decoder (&_tx_message, &_rx_tracking);
            terminal_message_1 (0,0, &_radiodata, &_fanet_mac_1, &_rx_tracking);
        }

        _i_data++;
        if (_i_data == N_STATIONS)
            _i_data = 0;
        _data_timer = INTERVAL_DATA;

    }
    _data_timer--;

    if (!_name_timer)
    {
        _tx_message.m_length = 0;
        _fanet_mac.type	= 2;
        _fanet_mac.s_manufactur_id	= source_manufacturer_id;
        _fanet_mac.s_unique_id		= source_unique_id[_i_name];

        strcpy(_tx_weather_data.id_station, stations_id[_i_name]);

        get_weather_station (stations_id[_i_name], &_tx_weather_data);

        strcpy(_tx_name.name, _tx_weather_data.short_name);
        _tx_name.n_length = strlen(_tx_name.name);

        type_2_name_coder (&_tx_message, &_tx_name);

        fanet_mac_coder (&_radiodata, &_fanet_mac, &_tx_message);

        // Monitor task
        //terminal_message_raw (1,0, &_radiodata, &_fanet_mac, &_tx_message);
        type_2_name_decoder (&_tx_message, &_rx_name);
        terminal_message_2 (true, false, &_radiodata, &_fanet_mac, &_rx_name);

        _i_name++;
        if (_i_name == N_STATIONS)
            _i_name = 0;
        _name_timer = INTERVAL_NAME;
    }
    _name_timer--;

}

sWeather this_station;
sWeather *this_station_data = &this_station;

int main (int argc, char *argv[])
{

    time_t now;
    struct tm *tm;

	char _second_new;
	char _second_old;
	//char _minute_new;
	//char _minute_old;

	boolean listen_only = false;

    if (argc > 1) {
        if (0==strcmp(argv[1], "-dbinit")) {
            db_init("FANET_Station.db");
            exit(0);
        } else if (0==strcmp(argv[1], "-l")) {
            listen_only = true;
            printf("-- only listening --\n");
        } else if (0==strcmp(argv[1], "-gti")) {
            b_send_gti = true;
            printf("-- Sending GTI --\n");
    }
    }
    db_login("FANET_Station.db");
    get_station_parameters(this_station_data);

   // if (argc < 2) {
    //    printf ("Usage: argv[0] sender|rec [message]\n");
     //   exit(1);
   // }

    init_fanet_radio(1000, true);

    //start_screen();
    terminal_start_screen(sf, 250, freq);

    while(1)
    {
		now = time(0);
		tm = localtime (&now);
		_second_new = tm->tm_sec;
		//_minute_new = tm->tm_min;

		system_data();
		receivepacket();

		if (_second_new!=_second_old)
		{
		  _second_old = _second_new;

		  if (listen_only == false) {

              fanet_service_scheduler();
              fanet_t3_messenger_scheduler();
		  }

		}

		/*if (_minute_new!=_minute_old)
		{
			_minute_old = _minute_new;
			show_register();
		}*/

        delay(1);
	}
    return (0);
}
