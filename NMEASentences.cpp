
#include "NMEASentences.h"
#include "NMEA.h"

#include <cstring>

/**
 * Utility macro to advance the pos pointer to after the next ','
 * and return -1 if done
 */
#define NextNMEAField() pos = strchr(pos, ','); if(pos == NULL) {return -1;}pos++
/**
 * Assert that a given field is not INT32_MAX. Else, return a rc
 */
#define CheckFieldValid(field, rc) if((field) == INT32_MAX) {return rc;}(void)0;


int parseGLLSentence(const char* buf, NMEAPosition* position) {
    const char* pos = buf;
    //Parse latitude
    NextNMEAField();
    position->latitude = parseNMEACoordinate(pos);
    CheckFieldValid(position->latitude, -2);
    //Parse latitude direction
    NextNMEAField();
    if(applyDirectionSignToCoordinate(*pos, &position->latitude)) {
        return -3;
    }
    //Parse longitude
    NextNMEAField();
    position->longitude = parseNMEACoordinate(pos);
    CheckFieldValid(position->longitude, -4);
    //Parse longitude direction
    NextNMEAField();
    if(applyDirectionSignToCoordinate(*pos, &position->longitude)) {
        return -5;
    }
    return 0;
}

int parseRMCSentence(const char* buf, RMCSentence* result) {
    const char* pos = buf;
    NextNMEAField();
    result->utcTime = parseNMEAUTCTime(pos);
    CheckFieldValid(result->utcTime, -2);
    //Parse status
    NextNMEAField();
    result->status = *pos;
    if(result->status != 'A' && result->status != 'V') {
        return -3;
    }
    //Parse latitude
    NextNMEAField();
    result->position.latitude = parseNMEACoordinate(pos);
    CheckFieldValid(result->position.latitude, -4);
    //Parse latitude direction
    NextNMEAField();
    if(applyDirectionSignToCoordinate(*pos, &result->position.latitude)) {
        return -5;
    }
    //Parse longitude
    NextNMEAField();
    result->position.longitude = parseNMEACoordinate(pos);
    CheckFieldValid(result->position.longitude, -6);
    //Parse longitude direction
    NextNMEAField();
    if(applyDirectionSignToCoordinate(*pos, &result->position.longitude)) {
        return -7;
    }
    //Parse speed
    NextNMEAField();
    result->speed = parseNMEAFixedPointDecimal(pos, 3);
    CheckFieldValid(result->speed, -8);
    //Parse course
    NextNMEAField();
    result->course = parseNMEAFixedPointDecimal(pos, 3);
    CheckFieldValid(result->course, -9);
    //Parse date
    NextNMEAField();
    result->date = parseNMEAFixedPointDecimal(pos, -1);
    CheckFieldValid(result->date, -10);
    //Ignore 2 magnetic variation fields
    NextNMEAField();
    NextNMEAField();
    //Position/fix mode
    NextNMEAField();
    result->posMode = *pos;
    if(result->posMode != 'N'
        && result->posMode != 'E'
        && result->posMode != 'A'
        && result->posMode != 'D') {
        return -11;
    }
    return 0;
}
