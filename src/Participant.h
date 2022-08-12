#include <string>

using namespace std;

class ParticipantInfo{
    private:
        string hostname; 
        string mac; 
        string ip;
        bool status; //TRUE = AWAKEN --- FALSE = ASLEEP

    public:
        ParticipantInfo(string _hostname,string _mac,string _ip,bool _status)
        {
            hostname = _hostname;
            mac = _mac;
            ip = _ip;
            status = _status;
        }
        string getHostname()
        {
            return hostname;
        }
        string getMac()
        {
            return mac;
        }
        string getIp()
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

