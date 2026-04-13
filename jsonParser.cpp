#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "json.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]){
    if (argc != 6) {
        std::cerr << "Error: Incorrect number of arguments\n" 
                  << "Expected Format: './jsonParser file1 file2 file3 file4 file5'\n\n"
                  << "This program is for the ECE146 Tekken 8 Project, where each controlled"
                  << " environment is ran 5 times. Each argument should be a '.json' file"
                  << " containing information retrieved from Wireshark\n"; 
    }

    std::ifstream file1(argv[1]);
    std::ifstream file2(argv[2]);
    std::ifstream file3(argv[3]);
    std::ifstream file4(argv[4]);
    std::ifstream file5(argv[5]);

    const json db1 = json::parse(file1);
    const json db2 = json::parse(file2);
    const json db3 = json::parse(file3);
    const json db4 = json::parse(file4);
    const json db5 = json::parse(file5);
    json dbs[] = {db1, db2, db3, db4, db5};
   

    for (int i = 0; i < 5; i++) {
        for (const auto single_object : dbs[i]) {
            // Bits per second from address to address
            const std::string AtoB_pps_str = single_object["Bits/s A → B"];
            const std::string BtoA_pps_str = single_object["Bits/s B → A"];
            const int AtoB_pps = std::stoi(AtoB_pps_str);
            const int BtoA_pps = std::stoi(BtoA_pps_str);

            // Packets
            const std::string totalPackets_str = single_object["Total Packets"];
            const int totalPackets = std::stoi(totalPackets_str);

            // Duration and Throughput
            const std::string duration_str = single_object["Duration"];
            const float duration = std::stod(duration_str);
            const float throughput_bps = AtoB_pps + BtoA_pps;
            const double throughput_mbps = std::round((throughput_bps / 1000000) * 100) / 100.0;
            
            // Packets per second
            // Expected packet rate should be the average PPS out of 5 baseline runs
            // Sometimes there might be a negative value, so we have a tolerance of 3% or .03
            const int packetPerSecond = std::round(totalPackets / duration);
            const float packetLoss = // TODO: Replace Expected with baseline average PPS
                std::max(0, ((Expected - packetPerSecond) / Expected) - 0.03) * 100;


            // Displaying statistics
            std::cout << "Run #" << i << ":\n"
                      << "Total Packets: " << totalPackets << "\n"
                      << "Wireshark Capture Duration: " << duration << "s\n"
                      << "Bits/s A → B: " << AtoB_pps << " bps\n"
                      << "Bits/s B → A: " << BtoA_pps << " bps\n\n"
                      << "Total Throughput: " << throughput_bps << " bps   |   "
                                              << throughput_mbps << " mbps" << "\n"
                      << "Packets per second: " << packetPerSecond 
                      << "Packet loss rate: " << packetLoss << "\n\n"
                      << "----------------------------------------\n\n";
        }
    }
    
    return 0;
}
