/**
 * UBLOX NMEA driver. Does not support binary protocol for simplicity
 * Reference: https://www.u-blox.com/sites/default/files/products/documents/u-blox7-V14_ReceiverDescrProtSpec_(GPS.G7-SW-12001)_Public.pdf
 */
#ifndef __UBLOX_H
#define __UBLOX_H

#include <stdint.h>
#include <string.h>
#include <ctype.h>

void ubloxLLDWrite(void* serialDriver, const char* buf, size_t size);
size_t ubloxLLDRead(void* serialDriver, char* buf, size_t size);

#define UBLOX_BUFSIZE 256
static char rxbuf[UBLOX_BUFSIZE];

/**
 * Read until \n. Returns the size if a whole line
 * has been read and the first character is $, i.e.
 * the buffer possibly contains some kind of valid NMEA message.
 * In case of read errors, returns 0.
 * In case of success, returns the number of valid
 * characters in the buffer, including \r\n.
 * When a non-zero value is returned, the buffer contains a valid cstring.
 */
size_t ubloxReadLine(void* port) {
    for(size_t i = 0; i < (UBLOX_BUFSIZE - 1); i++) {
        if(ubloxLLDRead(port, &rxbuf[i], 1) == 0) {
            return 0;
        }
        if(i == 0 && rxbuf[0] != '$') {
            return 0;
        }
        if(rxbuf[i] == '\n') {
            //Frame finished. Perform some validity checks
            if(i < 3) { //'$' + 1+ byte content + \r\n
                return 0;
            }
            if(rxbuf[i - 1] != '\r') { //End must be \r\n
                return 0;
            }
            //Return valid frame
            rxbuf[i + 1] = '\0'; //Ensure buffer contains a cstring
            return i + 1;
        }
    }
    //Buffer full but no LF founds
    return 0;
}

void parseUBloxMessage(size_t size) {
    if(strncmp("$GPGLL,", rxbuf, size)) {
        GPSPosition pos;
        parseGLLMessage(size, &pos);
    }
}

/**
 * Compute the UBlox checksum and place both
 * checksum bytes after the payload.
 * Refer to UBlox7 refman section 19 "UBX Checksum" for details
 * @param payload The payload.
 *    Must have payloadSize plus two bytes available (where the checksum is placed).
 *    The payload itself is not modified (but the checksum bytes are)
 * @param payloadSize The number of valid bytes in payload.
 *    This must be <= the number of available bytes in payload minus two.
 */
void calcChecksum(char* payload, size_t payloadSize) {
  uint8_t checksum[2] = {0, 0};
  for (size_t i = 0; i < payloadSize ;i++) {
    checksum[0] += payload[i];
    checksum[1] += checksum[0];
  }
  memcpy(payload + payloadSize, checksum, 2);
}

/**
 * Takes
 */
void sendNMEAMsgAndChecksum(void* serialPort, const char* msg, size_t size) {
    //Write the msg itself
    ubloxLLDWrite(serialPort, msg, size);
    //Compute and send checksum
    uint16_t chksum = computeHexNMEAChecksum(msg + 1, size - 2); //
    ubloxLLDWrite(serialPort, (const char*)&chksum, 2);
    ubloxLLDWrite(serialPort, "\r\n", 2);
}

void configureUBLOX(void* port) {
    /**
     * Port ID: 1 = UART
     * Inproto / outproto: NMEA only
     * Baudrate: 115200
     * Autobauding off
     * Flags: None
     */
    static const char msg[] = "$PUBX,41,1,0001,0001,115200,0*";
    //Send message, then compute and send checksum
    sendNMEAMsgAndChecksum(port, msg, strlen(msg));
}


#endif //__UBLOX_H