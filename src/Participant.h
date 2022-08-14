
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
    ParticipantInfo(std::string, std::string, std::string, bool);

    std::string getHostname();

    std::string getMac();

    std::string getIp();

    bool getStatus();

    void setStatus(bool);
};

#endif