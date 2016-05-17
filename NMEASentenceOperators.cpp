#include "NMEASentenceOperators.h"

#include <iomanip>

using namespace std;

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

bool operator!=(NMEAPosition const &a, NMEAPosition const &b) {
    return !(a == b);
}

bool operator==(GSVSatInfo const &a, GSVSatInfo const &b) {
    return a.id == b.id && a.azimuth == b.azimuth
            && a.elevation == b.elevation && a.signal == b.signal;
}

bool operator!=(GSVSatInfo const &a, GSVSatInfo const &b) {
    return !(a == b);
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

bool operator!=(RMCSentence const &a, RMCSentence const &b) {
    return !(a == b);
}

bool operator==(GSVSentence const &a, GSVSentence const &b) {
    if(a.numMsgs != b.numMsgs
         || a.msgNum != b.msgNum
         || a.numSats != b.numSats
         || a.numSatInfos != b.numSatInfos) {
        return false;
    }
    //Compare satellite infos
    for (int i = 0; i < a.numSatInfos; ++i) {
        if(a.satellites[i] != b.satellites[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(GSVSentence const &a, GSVSentence const &b) {
    return !(a == b);
}

std::ostream& operator<<(std::ostream &os, GSVSatInfo const &m) {
    os << "Satellite { #" << (int)m.id << ", azimuth " << (int)m.azimuth
       << ", elevation " << (int)m.elevation << ", signal " << ((int)m.signal) << '}';
    return os;
}

std::ostream& operator<<(std::ostream &os, GSVSentence const &m) {
    os << "GSVSentence { #" << (int)m.msgNum << " of "
       << (int)m.numMsgs << ", " << (int)m.numSats << " satellites in view: ";
    for (int i = 0; i < m.numSatInfos; ++i) {
        if(i != 0) {
            os << ", ";
        }
        os << m.satellites[i];
    }
    os << " }";
    return os;
}