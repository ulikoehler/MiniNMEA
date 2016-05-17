#define BOOST_TEST_MODULE NMEA
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "NMEA.h"
#include "NMEASentences.h"
#include "NMEASentenceOperators.h"

using namespace std;


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

BOOST_AUTO_TEST_CASE(TestParseNMEAInteger)
{
    BOOST_CHECK_EQUAL(12355912, parseNMEAInteger("12355912"));
    BOOST_CHECK_EQUAL(1234, parseNMEAInteger("01234"));
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
    //West
    memset(&pos, 0, sizeof(NMEAPosition));
    ref = {475395225, -100736179};
    msg = "$GPGLL,4753.95225,N,01007.36179,W,133017.00,A,A*";
    BOOST_CHECK_EQUAL(0, parseGLLSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
    //South
    memset(&pos, 0, sizeof(NMEAPosition));
    ref = {-475395225, 100736179};
    msg = "$GPGLL,4753.95225,S,01007.36179,E,133017.00,A,A*";
    BOOST_CHECK_EQUAL(0, parseGLLSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
    //South & West
    memset(&pos, 0, sizeof(NMEAPosition));
    ref = {-475395225, -100736179};
    msg = "$GPGLL,4753.95225,S,01007.36179,W,133017.00,A,A*";
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

BOOST_AUTO_TEST_CASE(TestParseGSVSentence)
{
    //Simple example
    GSVSentence pos, ref = {3, 1, 10, 4, {{23,38,230,44}, {29,71,156,47}, {7,29,116,41}, {8,9,81,36}} };
    const char* msg = "$GPGSV,3,1,10,23,38,230,44,29,71,156,47,07,29,116,41,08,09,081,36*7F";
    BOOST_CHECK_EQUAL(0, parseGSVSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
    //Simple example with only 2 sats (checksum is wrong, don't care)
    memset(&pos, 0, sizeof(GSVSentence));
    ref = {3, 1, 10, 2, {{23,38,230,44}, {29,71,156,47}} };
    msg = "$GPGSV,3,1,10,23,38,230,44,29,71,156,47*7F";
    BOOST_CHECK_EQUAL(0, parseGSVSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
    //Simple example with no sats (checksum is wrong, don't care)
    memset(&pos, 0, sizeof(GSVSentence));
    ref = {3, 1, 10, 0, {} };
    msg = "$GPGSV,3,1,10*7F";
    BOOST_CHECK_EQUAL(0, parseGSVSentence(msg, &pos));
    BOOST_CHECK_EQUAL(ref, pos);
}


