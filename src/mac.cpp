#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

std::string exec() {
    std::array<char, 128> buffer;
    std::string result,mac;
    int i,start;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("arp -a", "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    start = result.find(":") - 2;


    for(i = start ;i < start+17;i++)
    {
        mac += result[i];
    }
    return mac;
}
