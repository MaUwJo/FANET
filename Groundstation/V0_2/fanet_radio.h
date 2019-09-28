/*
 * fanet_radio.h
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

#ifndef FANET_RADIO_H
#define FANET_RADIO_H


#include "fanet_struct.h"
#include "fanet_terminal.h"

extern sRadioData	radio_data;

// FANET center frequnecy - EU area
//unsigned int  freq = 868200000; // in Hz! (868.2) 

void selectreceiver();

void unselectreceiver();

// Routine to read one byte from the SPI bus
byte readReg(byte addr);

// Routine to write one byte to the SPI bus
void writeReg(byte addr, byte value);

void write_tx_buffer(byte addr, byte *value, byte len);

void writeFIFO(byte addr, sRawMessage *_tx_message); 

void show_register (void);

void configPower (int8_t pw);

void opmode (byte mode);

void read_radio_data (sRadioData *radiodata);

void radio_info (sSystem *_system_data);

boolean read_rx_data(sRawMessage *_rx_radio, sRadioData *_radiodata);

void tx_radio_values (sRadioData *_radiodata);

void write_tx_data(sRadioData *_radiodata, sRawMessage *_tx_message);

void init_rpi_spi(void);

void setup_sx1276_LoRa (void);

void init_fanet_radio_b();

void init_fanet_radio(int _delay, boolean _show_msg);

#endif

