/**
 * Core NMEA parser components
 */
#ifndef __NMEA_H
#define __NMEA_H

#include <cstdint>
#include <cstdlib>

/**
 * Parse a coordinate of the form 4153.94820
 * to an integer, e.g. 415394820 so that the result
 * divided by 1e7 equals the degrees and the result
 * modulo 1e7 equals 1/1e5 of the minutes.
 *
 * Assumes that trailing zeros are not omitted.
 * Returns INT32_MAX on error.
 */
int32_t parseNMEACoordinate(const char* src);

/**
 * Parse a UTC time in the form 083559.00
 */
int32_t parseNMEAUTCTime(const char* src);

/**
 * Generic fixed point decimal parser.
 * Parses e.g. "1234.56" to 1234560 (with decimals = 3)
 * Automatically fixes missing trailing zeroes given the number of
 * expected decimals. Does not parse signs.
 *
 * 
 * 
 * @param decimals. How many digits to expect after the dot. Set to -1 to ignore decimals.
 * @return The parsed number or INT32_MAX in case of errors
 */
int32_t parseNMEAFixedPointDecimal(const char* src, int decimals);

/**
 * Compute the NMEA checksum of a cstring.
 * The first character (usually $) and the last (usually *) are ignored.
 */
uint8_t computeNMEAChecksum(const char* payload);

/**
 * Compute the NMEA checksum of a length-specified string.
 * The first character (usually $) and the last (usually *) are ignored,
 * but the size and the pointer must include both characters.
 */
uint8_t computeNMEAChecksum(const char* payload, size_t size);

/**
 * Parse a single-character N/E/S/W direction.
 * Applies the direction to the given integer:
 *   - For S/W, negates the direction
 *   - For N/E, does not change the direction
 * Otherwise, does not change the value.
 * @return 0 on success, -n else.
 */
int applyDirectionSignToCoordinate(char direction, int32_t* target);

/**
 * Compute the NMEA checksum and convert it into a two-byte hex checksum in ASCII form.
 * The resulting byte can be copied directly to the output stream.
 */
uint16_t computeHexNMEAChecksum(const char* payload, size_t size);

/**
 * Same as computeHexNMEAChecksum(const char*, size_t), but determines the size using strlen.
 */
uint16_t computeHexNMEAChecksum(const char* payload);

#endif //__NMEA_H