/**
 * Specific NMEA sentence parsers
 * Developed and tested with UBlox 7 chipset.
 */
#ifndef __NMEA_SENTENCES_H
#define __NMEA_SENTENCES_H

#include <cstdint>

/**
 * Represents the current WGS84 2D position to 1/100000 minute resolution
 */
struct NMEAPosition {
    /**
     * Degrees component of latitude and longitude.
     * Positive means north/east, Negative means south/west.
     * The value divided by 1e7 equals the degrees and the result
     * modulo 1e7 equals 1/1e5 of the minutes. 
     */
    int32_t latitude, longitude;
};

struct RMCSentence {
    /**
     * UTC time as hhmmss.ss
     */
    uint32_t utcTime;
    /**
     * Mainly: A = Fix (including dead reckoning), V = No fix
     */
    char status;
    /**
     * Current position.
     * Fields are INT32_MAX if not valid.
     */
    NMEAPosition position;
    /**
     * Speed over ground in 1/1000 knot.
     * INT32_MAX if invalidN=No Fix, E=Estimated/Dead Reckoning Fix, A=Autonomous GNSS Fix, D=Differential GNSS Fix
     */
    int32_t speed;
    /**
     * Course over ground in 1/1000 degrees
     */
    int32_t course;
    /**
     * Date in ddmmyy format
     */
    int32_t date;
    /**
     * Position mode:
     *  - N: No fix
     *  - E: Estimated / Dead reckoning
     *  - A: Autonomous GNSS fix
     *  - D: Differential fix
     */
    char posMode;
};

int parseGLLSentence(const char* buf, NMEAPosition* position);
int parseRMCSentence(const char* buf, RMCSentence* result);

#endif //__NMEA_SENTENCES_H