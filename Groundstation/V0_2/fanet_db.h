/*
 * fanet_db.h
 *
 * Copyright 2018  <pi@RPi3B_FANET_2>
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

#ifndef FANET_DB_H
#define FANET_DB_H

#include "fanet_radio.h"
#include "fanet_struct.h"

void db_init (char *_db_name);

void db_login (char *_db_name);

void get_station_parameters (sWeather *_station_data);

void get_weather_data (sWeather *_weather_data);

void get_weather_station (const char* station_id, sWeather *_weather_data);

void write_system_data_1min (sSystem *_system_data);

void write_system_data_15min (sSystem *_system_data);

void write_object_tracking (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sTRACKING *_tracking);
void write_object_ground_tracking (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sGroundTracking *_tracking);

void write_object_name (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sName *_name);

void update_routing_table (sRouting *_routing_data);

void write_message_send (sMessage *_message);

void search_last_welcome_message (sMessage *_message);

void search_online_address (sOnline *_online);

void delete_old_data (void);

#endif
