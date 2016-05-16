#define BOOST_TEST_MODULE NMEA
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "NMEA.h"
#include "NMEASentences.h"

using namespace std;

/**
 * Print an int32_t coordinate as xx°yy.zzzzz"
 */
std::ostream& printCoordinate(std::ostream &os, int32_t coord) {
    //All-integer computation avoids floating point rounding errors
    div_t degmin = div(coord, 10000000);
    div_t minutes = div(degmin.rem, 100000);
    os << degmin.quot << "°" << minutes.quot << '.' << minutes.rem << '"';
    return os;
}

std::ostream& operator<<(std::ostream &os, NMEAPosition const &m) {
    os << "NMEAPosition { ";
    printCoordinate(os, abs(m.latitude)) << (m.latitude < 0 ? 'S' : 'N') << ' ';
    printCoordinate(os, abs(m.longitude)) << (m.latitude < 0 ? 'W' : 'E') << " }";
    return os;
}

std::ostream& operator<<(std::ostream &os, RMCSentence const &m) {
    os << "RMC { ";
    //Date (ISO 8601)
    div_t year = div(m.date, 100);
    div_t month = div(year.quot, 100);
    //os << m.date << " ";
    os << setfill('0') << "20" << setw(2) << year.rem
       << '-' << setw(2) << month.rem << "-" << setw(2)
       << month.quot << ' ';
    //Time
    div_t subseconds = div(m.utcTime, 100);
    div_t seconds = div(subseconds.quot, 100);
    div_t minutes = div(seconds.quot, 100);
    os << minutes.quot << ':' << minutes.rem << ':' << seconds.rem << '.' << subseconds.rem << ", ";
    //Position
    os << m.position;
    //Status & posMode
    os << m.status << '/' << m.posMode << ", ";
    //Speed & course
    os << (m.speed / 1000.0) << " kt, " << (m.course / 1000.0) << "° }";
    return os;
}

bool operator==(NMEAPosition const &a, NMEAPosition const &b) {
    return a.latitude == b.latitude && a.longitude == b.longitude;
}

bool operator==(RMCSentence const &a, RMCSentence const &b) {
    return a.utcTime == b.utcTime
            && a.position == b.position
            && a.status == b.status
            && a.speed == b.speed
            && a.course == b.course
            && a.date == b.date
            && a.posMode == b.posMode;
}

BOOST_AUTO_TEST_CASE(TestParseNMEACoordinate)
{
    BOOST_CHECK_EQUAL(415394820, parseNMEACoordinate("4153.94820"));
    BOOST_CHECK_EQUAL(415394820, parseNMEACoordinate("4153.9482"));
    BOOST_CHECK_EQUAL(415394800, parseNMEACoordinate("4153.948"));
    BOOST_CHECK_EQUAL(415394820, parseNMEACoordinate("4153.94820,"));
    BOOST_CHECK_EQUAL(1795394820, parseNMEACoordinate("17953.94820"));
    BOOST_CHECK_EQUAL(INT32_MAX, parseNMEACoordinate("415394820"));
    BOOST_CHECK_EQUAL(INT32_MAX, parseNMEACoordinate("415394820,"));
    BOOST_CHECK_EQUAL(INT32_MAX, parseNMEACoordinate(""));
    BOOST_CHECK_EQUAL(INT32_MAX, parseNMEACoordinate(","));
}

BOOST_AUTO_TEST_CASE(TestParseNMEAUTCTime)
{
    BOOST_CHECK_EQUAL(12355912, parseNMEAUTCTime("123559.12"));
}

BOOST_AUTO_TEST_CASE(TestNMEAChecksum)
{
    //Examples from the UBlox 7 manual
    BOOST_CHECK_EQUAL(0x33, computeNMEAChecksum("$PUBX,00*"));
    BOOST_CHECK_EQUAL(0x33, computeNMEAChecksum("$PUBX,00*", 9));
    BOOST_CHECK_EQUAL(0x24, computeNMEAChecksum("$EIGNQ,RMC*"));
    BOOST_CHECK_EQUAL(0x5B, computeNMEAChecksum("$GPGBS,235458.00,1.4,1.3,3.1,03,,-21.4,3.8*"));
    BOOST_CHECK_EQUAL(0x06, computeNMEAChecksum("$GPVTG,77.52,T,,M,0.004,N,0.008,K,A*"));
    BOOST_CHECK_EQUAL(0x64, computeNMEAChecksum("$GPZDA,082710.00,16,09,2002,00,00*"));
}

BOOST_AUTO_TEST_CASE(TestNMEAHexChecksum)
{
    //Examples from the UBlox 7 manual, manually converted to ASCII Hex code.
    //Note that characters are inversed in binary reference representation
    BOOST_CHECK_EQUAL(0x3333, computeHexNMEAChecksum("$PUBX,00*"));
    BOOST_CHECK_EQUAL(0x3432, computeHexNMEAChecksum("$EIGNQ,RMC*"));
    BOOST_CHECK_EQUAL(0x3436, computeHexNMEAChecksum("$GPZDA,082710.00,16,09,2002,00,00*"));
}

BOOST_AUTO_TEST_CASE(TestParseGLLSentence)
{
    //Simple example
    NMEAPosition pos, ref = {475395225, 100736179};
    const char* msg = "$GPGLL,4753.95225,N,01007.36179,E,133017.00,A,A*";
    BOOST_CHECK_EQUAL(0, parseGLLSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
}


BOOST_AUTO_TEST_CASE(TestParseRMCSentence)
{
    //Simple example
    RMCSentence pos, ref = {8355900, 'A', {471711437, 83391522}, 4, 77520, 91202, 'A'};
    const char* msg = "$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*";
    BOOST_CHECK_EQUAL(0, parseRMCSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
}


