
#ifndef Participant_H
#define Participant_H

#include <string>

class ParticipantInfo
{
    std::string hostname;
    std::string mac;
    std::string ip;
    bool status; // TRUE = AWAKEN --- FALSE = ASLEEP

public:
    ParticipantInfo(std::string _hostname, std::string _mac, std::string _ip, bool _status)
    {
        hostname = _hostname;
        mac = _mac;
        ip = _ip;
        status = _status;
    }
    std::string getHostname()
    {
        return hostname;
    }
    std::string getMac()
    {
        return mac;
    }
    std::string getIp()
    {
        return ip;
    }
    bool getStatus()
    {
        return status;
    }
    void setStatus(bool _status)
    {
        status = _status;
    }
};

#endif