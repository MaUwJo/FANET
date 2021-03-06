/*
 * fanet_radio.c
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

#ifndef FANET_RADIO_C
#define FANET_RADIO_C


/***********************************************************************
 * FANET radio
 * 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |03.11.2018|  bet | Init Version
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "sx1276Regs-LoRa.h"

#include "fanet_struct.c"
#include "fanet_terminal.c"

typedef unsigned char byte;

static const int CHANNEL = 0;

// SX1276 - Raspberry connections
int ssPin = 6;
int dio0  = 7;
int RST   = 0;

// Set FANET center frequency
unsigned int  freq = 868200000; // in Hz! (868.2)

sRadioData	radio_data;

void selectreceiver()
{
    digitalWrite(ssPin, LOW);
}

void unselectreceiver()
{
    digitalWrite(ssPin, HIGH);
}

// Routine to read one byte from the SPI bus
byte readReg(byte addr)
{
    unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}

// Routine to write one byte to the SPI bus
void writeReg(byte addr, byte value)
{
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();
}


void write_tx_buffer(byte addr, byte *value, byte len) {                                                       
    unsigned char spibuf[256];                                                                          
    spibuf[0] = addr | 0x80;                                                                            
    for (int i = 0; i < len; i++) {                                                                         
        spibuf[i + 1] = value[i];                                                                       
    }                                                                                                   
    selectreceiver();                                                                                   
    wiringPiSPIDataRW(CHANNEL, spibuf, len + 1);                                                        
    unselectreceiver();                                                                                 
}


void writeFIFO(byte addr, sRawMessage *_tx_message) 
{                                                       
    unsigned char spibuf[256];                                                                          
    spibuf[0] = addr | 0x80;                                                                            
    for (int i = 0; i < _tx_message->m_length; i++) {                                                                         
        spibuf[i + 1] = _tx_message->message[i];                                                                       
    }                                                                                                   
    selectreceiver();                                                                                   
    wiringPiSPIDataRW(CHANNEL, spibuf, _tx_message->m_length + 1);                                                        
    unselectreceiver();                                                                                 
}

/***********************************************************************
 * Reads out the SX1276 registers and shows it on the screen
 * It's a debug tool. Not needed for regualar work. 
 * *********************************************************************
 * Ver | Date     | Sign | Describtion
 * ---------------------------------------------------------------------
 * 0.1 |03.11.2018|  bet | Init Version
 **********************************************************************/
void show_register (void)
{
	int _x = 0;
	int _y = 0;
	
	printf ("SX1276 register...\n");
	printf ("     _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _a _b _c _d _e _f\n");
	printf ("   -------------------------------------------------\n");

	while (_y < 16)
	{
		printf ("%1x_ | ",_y);
		while (_x < 16)
		{
			printf ("%02x ",	readReg(_y*16+_x));	
			_x++;
		}
		printf ("\n");
		_x = 0;
		_y++;
	}
}


void configPower (int8_t pw)
{
	if(pw >= 15)
		pw = 15;
    else if(pw < 2)
		pw = 2;

	// check board type for BOOST pin
	writeReg(REG_LR_PACONFIG, (uint8_t)(0x80|(pw&0x0f)));
	writeReg(REG_LR_PADAC, readReg(REG_LR_PADAC)|0x04);

}


void opmode (byte mode)
{
    writeReg(REG_LR_OPMODE, (readReg(REG_LR_OPMODE) & RFLR_OPMODE_MASK) | mode);	// Prüfen !!!!!!!!!!!!!!!
}


void read_radio_data (sRadioData *radiodata)
{
	char	_cr;
	int64_t _freq_err;
	
	radiodata->rssi 		= readReg(REG_LR_RSSIVALUE)-157;		// Current RSSI value (dBm), RSSI[dBm] = -157 + Rssi (using HF output port)

	radiodata->psnr			= (int8_t)(readReg(REG_LR_PKTSNRVALUE))*.25;	// Estimation of SNR on last packet received.In two’s compliment format mutiplied by 4.

	radiodata->prssi		= readReg(REG_LR_PKTRSSIVALUE)-157;
	if (radiodata->psnr < 0)										// If SNR < 0 dB, correct Packet RSSI, see section 5.5.5 for details
		radiodata->prssi += radiodata->psnr;
		
	radiodata->rx_headers	= readReg(REG_LR_RXHEADERCNTVALUEMSB);	// Number of valid headers received since last transition into Rx mode
	radiodata->rx_headers <<= 8;
	radiodata->rx_headers  += readReg(REG_LR_RXHEADERCNTVALUELSB);

	radiodata->rx_packets	= readReg(REG_LR_RXPACKETCNTVALUEMSB);	// Number of valid headers received since last transition into Rx mode
	radiodata->rx_packets <<= 8;
	radiodata->rx_packets  += readReg(REG_LR_RXPACKETCNTVALUELSB);

	_freq_err 	= readReg(REG_LR_FEIMSB)&0x0F;			// Estimated frequency error from last header received
	_freq_err <<= 8;
	_freq_err  += readReg(REG_LR_FEIMID);
	_freq_err <<= 8;
	_freq_err  += readReg(REG_LR_FEILSB);
	
	if (_freq_err&0x080000)								// If bit = 1 the value is negtive
		_freq_err |= 0xFFFFFFFFFFF00000;
	
	_freq_err <<= 24;
	_freq_err = _freq_err / 32000000;
	_freq_err >>= 1;

	radiodata->freq_err = _freq_err;

	_cr = readReg(REG_LR_MODEMSTAT)&0xE0; 				//Coding rate of last header received
	_cr >>= 5;

	switch (_cr)
	{
		case 1: strcpy (radiodata->coding_rate, "4/5"); break;
		case 2: strcpy (radiodata->coding_rate, "4/6"); break;
		case 3: strcpy (radiodata->coding_rate, "4/7"); break;
		case 4: strcpy (radiodata->coding_rate, "4/8"); break;
		default: strcpy (radiodata->coding_rate, "-/-"); break;
	}
}

void radio_info (sSystem *_system_data)
{
	_system_data->tx_time_1min = radio_data.tx_time;
	_system_data->tx_packets_1min = radio_data.tx_packets;
	_system_data->rx_packets_1min = radio_data.rx_packets;
	
	radio_data.tx_time = 0;
	radio_data.tx_packets = 0;
	radio_data.rx_packets = 0;
}


boolean read_rx_data(sRawMessage *_rx_radio, sRadioData *_radiodata)
{
    boolean _rx_done = FALSE;
    
    if(digitalRead(dio0) == 1)						// Check if RxDone is set
    {
		_radiodata->timestamp = time(NULL);
		
		_rx_done = TRUE;

		//show_register();
		
		int irqflags = readReg(REG_LR_IRQFLAGS);
		
		writeReg(REG_LR_IRQFLAGS, 0x40);			// Clears RxDone IRQ flag

		read_radio_data(_radiodata);

		if((irqflags & 0x20) == 0x20)				// Check if payload has a CRC error
		{
			_radiodata->crc_err = 1;
			writeReg(REG_LR_IRQFLAGS, 0x20);		// Clears PayloadCrcError IRQ flag
			_rx_radio->m_length = 0;			
		}
		else
		{
			_radiodata->crc_err = 0;
		}
		
		byte currentAddr = readReg(REG_LR_FIFORXCURRENTADDR);	// Reads start address (in data buffer) of last packet received
		byte receivedCount = readReg(REG_LR_RXNBBYTES);			// Reads number of payload bytes of latest packet received
		_rx_radio->m_length = receivedCount;

		writeReg(REG_LR_FIFOADDRPTR, currentAddr);				// Sets SPI interface address pointer in FIFO data buffer.

		for(int i = 0; i < receivedCount; i++)
		{
			_rx_radio->message[i] = (char)readReg(REG_LR_FIFO);			// Reads FIFO data input/output byte byte
		}
		radio_data.rx_packets++;
	}	
	return _rx_done;
}



void tx_radio_values (sRadioData *_radiodata)
{
	// Fake date
	_radiodata->rssi = -120;
	_radiodata->prssi = 14;
	strcpy (_radiodata->coding_rate, "4/8");
}


void write_tx_data(sRadioData *_radiodata, sRawMessage *_tx_message)
{
	//sRadioData	_tx_radio;
	
	_radiodata->crc_err = false;
	
	tx_radio_values (_radiodata);
    
    // set the IRQ mapping DIO0=TxDone
    writeReg(REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01);
    // clear all radio IRQ flags
    writeReg(REG_LR_IRQFLAGS, 0xFF);

    // initialize the payload size and address pointers
    writeReg(REG_LR_FIFOTXBASEADDR, 0x00);
    writeReg(REG_LR_FIFOADDRPTR, 0x00);
    writeReg(REG_LR_PAYLOADLENGTH, _tx_message->m_length);

    // download buffer to the radio FIFO
    writeFIFO(REG_LR_FIFO, _tx_message);
    // now we actually start the transmission
    opmode(RFLR_OPMODE_TRANSMITTER);
    
    while (!(readReg(REG_LR_IRQFLAGS)&0x08))			// Wait until TxDone Interrupt ist set
    {
		delay(1);
		radio_data.tx_time++;
	}
    writeReg(REG_LR_IRQFLAGS, readReg(REG_LR_IRQFLAGS)|0x08);// Clear TxDone Interupt
	radio_data.tx_packets++;
	
    writeReg(REG_LR_LNA, RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_HF_ON);	
    // set the IRQ mapping DIO0=RxDone DIO1=NOP DIO2=NOP
    writeReg(REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00|RFLR_DIOMAPPING1_DIO1_00|RFLR_DIOMAPPING1_DIO2_00);
    // clear all radio IRQ flags
    writeReg(REG_LR_IRQFLAGS, 0xFF);

    opmode(RFLR_OPMODE_RECEIVER);
}

void init_rpi_spi(void)
{
    wiringPiSetup ();
    pinMode(ssPin, OUTPUT);
    pinMode(dio0, INPUT);
    pinMode(RST, OUTPUT);
    wiringPiSPISetup(CHANNEL, 500000);	
}

/******************************************************************
 * Setup up the SX1276 
 * 
 *****************************************************************/
void setup_sx1276_LoRa (void)
{
	byte version = readReg(REG_LR_VERSION);
		
	printf("SX1276: RESET Chip\n");
	digitalWrite(RST, LOW);
    delay(1);					// Manual Reset min. 100us resp. 1ms (Datasheet SX1276, 7.2.2. Manual Reset)
    digitalWrite(RST, HIGH);
    delay(10);					// Wait for 10ms (POR) or 5ms (Manual Reset) until Chip is ready (Datasheet SX1276, 7.2.1. POR, 7.2.2. Manual Reset)

	version = readReg(REG_LR_VERSION);
    if (version == 0x12)
    {
		// sx1276
		printf("SX1276 detected, starting.\n");
	} else {
		printf("Unrecognized transceiver.\n");
		printf("Version: 0x%x\n",version);
		exit(1);
	}
	
    writeReg(REG_LR_OPMODE, 0x80); 			// Set SX1276 into "Sleep mode", "LoRa" Mode, High Frequency Mode
    
    // set frequency
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    writeReg(REG_LR_FRFMSB, (byte)(frf>>16) );
    writeReg(REG_LR_FRFMID, (byte)(frf>> 8) );
    writeReg(REG_LR_FRFLSB, (byte)(frf>> 0) );

    writeReg(REG_LR_SYNCWORD, 0xF1); 		// FANET sync word is 0xF1
    
    writeReg(REG_LR_MODEMCONFIG3,0x04);	// LowDataRateOptimize = Disabled
										// AgcAutoOn = True (LNA gain set by the internal AGC loop)

    writeReg(REG_LR_MODEMCONFIG1,0x88);		// bw = 250kHz, CodingRate = 4/8, Header = Explicit Header mode
    
    writeReg(REG_LR_MODEMCONFIG2,0x74);		// SF = 7, TxContinuousMode = Disabled, CRC enabled
	
    writeReg(REG_LR_PAYLOADMAXLENGTH,0xFF);	// Set the FIFO to the maximum
    //writeReg(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);		// n/a if Explicit Header mode is set
    writeReg(REG_LR_HOPPERIOD,0x0);			// Disabled
    writeReg(REG_LR_FIFOADDRPTR, readReg(REG_LR_FIFORXBASEADDR));

    writeReg(REG_LR_LNA, 0x23);		// LAN G1 = maximum gain, Boost on - 150% LNA current 
}


void init_fanet_radio()
{
	init_rpi_spi();
	setup_sx1276_LoRa();

	opmode(RFLR_OPMODE_STANDBY);
    configPower(15);
    opmode(RFLR_OPMODE_RECEIVER);
	
	show_register();
	delay (2000);
}



#endif

