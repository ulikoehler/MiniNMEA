#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "UBlox.h"

using namespace std;
using namespace boost;

boost::asio::io_service io;

void ubloxLLDWrite(void* arg, const char* buf, size_t length) {
    boost::asio::serial_port* port = (boost::asio::serial_port*)arg;
    boost::asio::write(*port, boost::asio::buffer(buf, length));
}

size_t ubloxLLDRead(void* arg, char* buf, size_t size) {
    boost::asio::serial_port* port = (boost::asio::serial_port*)arg;
    return boost::asio::read(*port, boost::asio::buffer(buf, size));
}

int main() {
    cout << parseNMEACoordinate("4153.94820") << endl;

    boost::asio::serial_port serial(io, "/dev/ttyACM0");
    //cout << computeNMEAChecksum(msg, strlen(msg));
    //uint16_t chksum = computeHexNMEAChecksum(msg, strlen(msg));
    //string s((char*)&chksum, 2);
    //cout << s << endl;
    //configureUBLOX(&serial);
    //requestPosition(&serial);
    while(true) {
        size_t linesize = ubloxReadLine(&serial);
        if(linesize == 0) {
            continue;
        }
        string s(rxbuf, linesize);
        cout << s;
    }
    /*char c;
    std::string result;
    for(;;)
    {
        boost::asio::read(serial, boost::asio::buffer(&c,1));
        cout << c;
    }*/
}