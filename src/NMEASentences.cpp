
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
/**
 * Like NextNMEAField(), but does not return if no next field is found.
 * pos is NULL after the call if there is no next field
 */
#define OptionalNextNMEAField() pos = strchr(pos, ','); if(pos != NULL) {pos++;}(void)0

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
    result->date = parseNMEAInteger(pos);
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


int parseGSVSentence(const char* buf, GSVSentence* result) {
    const char* pos = buf;
    //Parse number of msgs
    NextNMEAField();
    int32_t numMsgs = parseNMEAInteger(pos);
    CheckFieldValid(numMsgs, -2);
    result->numMsgs = (uint16_t)numMsgs;
    //Parse current msg id
    NextNMEAField();
    int32_t msgNum = parseNMEAInteger(pos);
    CheckFieldValid(msgNum, -3);
    result->msgNum = (uint8_t)msgNum;
    //Parse satellites in view
    NextNMEAField();
    int32_t numSats = parseNMEAInteger(pos);
    CheckFieldValid(numSats, -4);
    result->numSats = (uint8_t)numSats;
    /*
     * Parse 1..4 satellite infos
     */ 
    result->numSatInfos = 0;
    for (int i = 0; i < 4; ++i) {
        OptionalNextNMEAField();
        if(pos == NULL) { //No next satellite
            break;
        }
        //We have a next record
        result->numSatInfos++;
        //Parse sat id
        int32_t satId = parseNMEAInteger(pos);
        CheckFieldValid(satId, -5);
        result->satellites[i].id = (uint16_t)satId;
        //Parse azimuth
        NextNMEAField();
        int32_t azimuth = parseNMEAInteger(pos);
        CheckFieldValid(azimuth, -6);
        result->satellites[i].azimuth = (int16_t)azimuth;
        //Parse elevation
        NextNMEAField();
        int32_t elevation = parseNMEAInteger(pos);
        CheckFieldValid(elevation, -7);
        result->satellites[i].elevation = (uint8_t)elevation;
        //Parse signal
        NextNMEAField();
        int32_t signalLevel = parseNMEAInteger(pos);
        result->satellites[i].signal = signalLevel == INT32_MAX ? UINT8_MAX : (uint8_t)signalLevel;
    }
    return 0;
}
