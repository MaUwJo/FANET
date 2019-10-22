/*
 *
*/

#include <stdio.h>          /* *printf(), *puts(), and friends */
#include <stdlib.h>         /* atof(), EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>         /* str*cpy() and friends */
#include <math.h>           /* round(), floor() */
#include <stdint.h>         /* uint16_t*/
#include <time.h>    /* struct tm, time_t, time(), gmtime() */

#include "aprs-is.h"

/*
#ifndef NO_APRSIS
#include "aprs-is.h"
#endif

#ifndef _WIN32
#include <getopt.h>
#include <mach/boolean.h>
#include <stdbool.h>

#else
#include "getopt-windows.h"
#endif

#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (a) : (b))
#endif
*/

//const char * dflPacket2 = "FNT04C520>APRS,qAS,FanGerPlz:/101000h4912.09N/00820.42Eg000/000/A=00345 !W45! id1E04C520\r\n";
const char * ppp               = "FNT110B0D>APRS,qAS,FanGerPlz:/%.2d%.2d%.2dh4912.17N/836.50Eg000/000/A=00345 id1E110B0D\r\n";
const char * dflPacketTemplate = "FNT04C520>APRS,qAS,FanGerPlz:/%.2d%.2d%.2dh4912.42N/00818.17Eg000/000/A=00345 !W45! id1E04C520\r\n‚";

int main(const int argc, const char** argv) {
	char         packetToSend[BUFSIZE] = "";

    time_t    t               = time(NULL);
    struct tm *now            = gmtime(&t); /* APRS uses GMT */

    if (argc > 1) {
        sprintf(packetToSend, "%s\r\n", argv[1]);
	} else {
        sprintf(packetToSend, ppp, now->tm_hour, now->tm_min, now->tm_sec);
	}
    char *server = "glidern5.glidernet.org";
    unsigned int port = 14580;
    char *username = "TTT";
    char *password = "29622";
    char *program  = "TTT";
    char *version  = "0.1";

    printf("-- sending: %s", packetToSend);
    if (strlen(server) && strlen(username) && strlen(password) && port != 0) {
	    sendPacket(server, port, username, password, program, version, packetToSend);
	}

	return EXIT_SUCCESS;
}
