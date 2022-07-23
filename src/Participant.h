#include <string>

using namespace std;

typedef struct __Participant{
    string hostname; 
    unsigned int mac[6]; 
    unsigned int ip[4];
    bool status; //TRUE = AWAKEN --- FALSE = ASLEEP
} Participant;

