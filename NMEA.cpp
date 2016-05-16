#include "NMEA.h"

#include <cctype>
#include <cstring>

int32_t parseNMEAFixedPointDecimal(const char* src, int decimals) {
    //There must be at least one digit at the start
    if(!isdigit(*src)) {
        return INT32_MAX;
    }
    int32_t ret = 0;
    int digitsSinceDot = -1; //0 if dot was just encountered
    for(;*src != ',' && *src != '\0';src++) {
        if(*src == '.') {
            if(digitsSinceDot != -1) {
                //Already seen dot, two dots are illegal
                return INT32_MAX;
            }
            digitsSinceDot = 0;
            continue;
        }
        // Fail if not a digit
        if(!isdigit(*src)) {
            return INT32_MAX;
        }
        //Parse digit
        ret = (ret * 10) + (*src - '0');
        if(digitsSinceDot != -1) { //Dot has been encountered
            digitsSinceDot++;
        }
    }
    if(digitsSinceDot == -1 && decimals > 0) { //No dot found but expected one
        return INT32_MAX;
    }
    //If there are not enough digits after the dot, we need to add the
    // appropriate number of zeros at the end.
    //This loop is ignored for decimals == -1
    while(digitsSinceDot++ < decimals) {
        ret *= 10;
    }
    return ret;
}

int32_t parseNMEACoordinate(const char* src) {
    return parseNMEAFixedPointDecimal(src, 5);
}

int32_t parseNMEAUTCTime(const char* src) {
    return parseNMEAFixedPointDecimal(src, 2);
}

uint8_t computeNMEAChecksum(const char* payload, size_t size) {
    if(size < 3) { //At least $ + 1 char content + * required
        return 0;
    }
    uint8_t checksum = payload[1];
    for (int i = 2; i < (size - 1); ++i) {
        checksum ^= payload[i];
    }
    return checksum;
}

uint8_t computeNMEAChecksum(const char* payload) {
    return computeNMEAChecksum(payload, strlen(payload));
}

static const char* hexLUT = "0123456789ABCDEF";

int applyDirectionSignToCoordinate(char direction, int32_t* target) {
    switch(direction) {
        case 'S':
        case 'W': {
            *target = -(*target);
            return 0;
        }
        case 'E':
        case 'N': return 0;
        default: return -1;
    }
}

uint16_t computeHexNMEAChecksum(const char* payload) {
    return computeHexNMEAChecksum(payload, strlen(payload));
}

uint16_t computeHexNMEAChecksum(const char* payload, size_t size) {
    uint8_t checksum = computeNMEAChecksum(payload, size);
    return hexLUT[checksum & 0x0F] << 8 | hexLUT[(checksum & 0xF0) >> 4];
}

/*NMEAArgs parseNMEAArguments(const char* buf) {
    NMEAArgs ret;
    //Parse buffer
    size_t argIndex = 0;
    size_t lastComma = 0; //
    while(*buf++ != '\0') {
        if(buf[0] == '*' //End of arguments, checksum follows
            || buf[0] == '\0') { //Buffer OVF prevention
            return ret;
        } else if(buf[i] == ',') {
            ret.args[argIndex].size = i - lastComma;
            lastComma = i;
            ret.args[argIndex++].offset = i + 1;
            ret.nargs = argIndex;
            if(argIndex >= MAX_NMEA_ARGS) {
                //Ignore additional arguments
                return ret;
            }
        }
    }
    return ret;
}*/
