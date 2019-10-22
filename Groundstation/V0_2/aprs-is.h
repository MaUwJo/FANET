/*
 aprs-is .h - based on - aprs-weather-submit version 1.3.1
 Copyright (c) 2019 Colin Cogle <colin@colincogle.name>
 
 This file, aprs-is.h, is part of aprs-weather-submit.
 
 aprs-weather-submit is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 aprs-weather-submit is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with aprs-weather-submit. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef NO_APRSIS

#ifndef aprs_is_h
#define aprs_is_h

/**
 * sendPacket() -- sends a packet to an APRS-IS IGate server.
 *
 * @author         Colin Cogle
 * @param server   The DNS hostname of the server.
 * @param port     The listening port on the server.
 * @param username The username with which to authenticate to the server.
 * @param password The password with which to authenticate to the server.
 * @param program  Program name to register
 * @param version  Program version to register
 * @param packet   The APRS-IS packet, as a string.
 * @since 0.3.1
 */
void sendPacket(const char* const restrict server, const unsigned short port,
                const char* const restrict username, const char* const restrict password,
                const char* const restrict program, const char* const restrict version,
                const char* const restrict packet);

/* This should be defined by the operating system, but just in case... */
#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

/* This is not defined on Windows by default, it seems. */
#ifndef EAI_SYSTEM
#define EAI_SYSTEM -11
#endif

/* A decent buffer size (plus one). */
#ifndef	BUFSIZE
#define BUFSIZE 1025
#endif


#endif /* aprs_is_h */

#endif // NO_APRSIS
