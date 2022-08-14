#pragma once
#include "Sockets.h"

#ifndef __Management__
#define __Management__

using namespace std;

string ReplaceAll(string str, const string& from, const string& to);
string createMagicPacket(string macAddress);
void sendWoL(vector<ParticipantInfo> *ParticipantsInfo, string hostname);
void receiveWoL();

#endif