/*
 * fanet_t7_tracking.c
 * 
 * Copyright 2019  <pi@pi4>
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

#ifndef FANET_T7_TRACKING_C
#define FANET_T7_TRACKING_C

#include <stdio.h>
#include "fanet_mac.h"
#include "fanet_struct.h"
#include "fanet_terminal.h"
#include "fanet_global.h"
#include "fanet_db.h"
#include "fanet_t1_tracking.h"

// 4913.1702,N,00822.4818,E 
// TODO: Get from configuration dynamically from geo-localisation
//#define STATION_LAT  49.131702   // Position of Ground Statation -> Is needed to calculate the distance to the tracked object ^M
//#define STATION_LON  8.224818    // Modfied if necessary^M


void decode_ground_tracking (sRawMessage *_rx_message, sGroundTracking *_tracking)
{
	if (_rx_message->message[_rx_message->m_pointer]&0x01)
		_tracking->tracking = 1;
	else
		_tracking->tracking = 0;			
}


void decode_ground_type (sRawMessage *_rx_message, sGroundTracking *_tracking)
{
	_tracking->ground_type = (_rx_message->message[_rx_message->m_pointer]&0xF0);
	_tracking->ground_type >>=4;

    strcpy(_tracking->ground_type_char, "----------------");
	switch (_tracking->ground_type){
		case  0:  strcpy (_tracking->ground_type_char, "Other"); break;
		case  1:  strcpy (_tracking->ground_type_char, "Walking"); break;
		case  2:  strcpy (_tracking->ground_type_char, "Vehicle"); break;
		case  3:  strcpy (_tracking->ground_type_char, "Bike"); break;
		case  4:  strcpy (_tracking->ground_type_char, "Boat"); break;
		case  8:  strcpy (_tracking->ground_type_char, "Need a ride"); break;
        case 12:  strcpy (_tracking->ground_type_char, "Need technical support"); break;
        case 13:  strcpy (_tracking->ground_type_char, "Need medical support"); break;
        case 14:  strcpy (_tracking->ground_type_char, "Distress call"); break;
        case 15:  strcpy (_tracking->ground_type_char, "Distress call automatically"); break;
		default: sprintf (_tracking->ground_type_char, "UNK ground type %d", _tracking->ground_type); break;
	}
}


void type_7_tracking_decoder (sRawMessage *_rx_payload, sGroundTracking *_rx_tracking)
{
	float _latitude;
	float _longitude;
	
	_rx_payload->m_pointer = 0;
	
	decode_abs_coordination (&_rx_payload->message[_rx_payload->m_pointer], &_latitude, &_longitude);
	_rx_tracking->latitude  = _latitude;
	_rx_tracking->longitude = _longitude;
	_rx_payload->m_pointer += 6;

	_rx_tracking->distance = distance (
	        this_station_data->latitude, this_station_data->longitude,
	        _rx_tracking->latitude, _rx_tracking->longitude, 'K');

	decode_ground_tracking (_rx_payload, _rx_tracking);
	decode_ground_type (_rx_payload, _rx_tracking);
}


void type_7_tracking_coder (sRawMessage *_tx_message, sTRACKING *_tx_tracking)
{
/* Not implemented yet
 * 
*/
}


void type_7_tracking_receiver (sRadioData *_radiodata, sFanetMAC *_fanet_mac, sRawMessage *_rx_payload)
{
    sGroundTracking	_rx_tracking;
	
	type_7_tracking_decoder (_rx_payload, &_rx_tracking);
	
	terminal_message_7 (0,0, _radiodata, _fanet_mac, &_rx_tracking);

	write_object_ground_tracking (_radiodata, _fanet_mac, &_rx_tracking);
}


#endif
