/**
 * C++ operator overloads (iostream <<, ==, !=) for NMEA sentence structures
 */
#ifndef __NMEA_SENTENCE_OPERATORS_H
#define __NMEA_SENTENCE_OPERATORS_H

#include <iostream>
#include <cstdint>

#include "NMEASentences.h"

/**
 * Print an int32_t coordinate as xx°yy.zzzzz"
 */
std::ostream& printCoordinate(std::ostream &os, int32_t coord);
std::ostream& operator<<(std::ostream &os, NMEAPosition const &m);
std::ostream& operator<<(std::ostream &os, RMCSentence const &m);
std::ostream& operator<<(std::ostream &os, GSVSatInfo const &m);
std::ostream& operator<<(std::ostream &os, GSVSentence const &m);

bool operator==(NMEAPosition const &a, NMEAPosition const &b);
bool operator!=(NMEAPosition const &a, NMEAPosition const &b);
bool operator==(GSVSatInfo const &a, GSVSatInfo const &b);
bool operator!=(GSVSatInfo const &a, GSVSatInfo const &b);
bool operator==(RMCSentence const &a, RMCSentence const &b);
bool operator!=(RMCSentence const &a, RMCSentence const &b);
bool operator==(GSVSentence const &a, GSVSentence const &b);
bool operator!=(GSVSentence const &a, GSVSentence const &b);

#endif //__NMEA_SENTENCE_OPERATORS_H