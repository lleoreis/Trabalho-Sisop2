#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
using namespace std;
class Tools
{
public:
    string getMacAddressAndHostname(char *placaRede)
    {
        array<char, 128> buffer;

        string result, mac, hostnameMacAddress, hostname;
        int i, start;
        string command = "cat /sys/class/net/";
        command.append(placaRede);
        command.append("/address");

        unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe)
        {
            throw runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
        start = result.find(":") - 2;

        for (i = start; i < start + 17; i++)
        {
            mac += result[i];
        }
        unique_ptr<FILE, decltype(&pclose)> pipe2(popen("hostname", "r"), pclose);
        if (!pipe2)
        {
            throw runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe2.get()) != nullptr)
        {
            hostname += buffer.data();
        }
        hostnameMacAddress.append(mac);
        hostnameMacAddress.append("|");
        hostnameMacAddress.append(hostname);

        return hostnameMacAddress;
    }
};