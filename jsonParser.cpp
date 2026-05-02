#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "json.hpp"

using json = nlohmann::json;

// Function to calculate the mean of all 3 elements of an array
double mean(double array[3]) {
    double result = 0.0;
    for (int i = 0; i < 3; i++) 
        result += array[i]; 

    return result / 3;
}


int main(int argc, char* argv[]){
    if (argc != 4) {
        std::cerr << "Error: Incorrect number of arguments\n" 
                  << "Expected Format: './jsonParser file1 file2 file3'\n\n"
                  << "This program is for the ECE146 Tekken 8 Project, where each controlled"
                  << " environment is ran 3 times. Each argument should be a '.json' file"
                  << " containing information retrieved from Wireshark\n\n\n";

        return 1;
    }

    for (int i = 0; i < 3; i++){
        std::ifstream test(argv[i+1]);
        if (!test.is_open()){
            std::cerr << "Error: Could not open file: " << argv[i+1] << "\n\n\n";
            return 1;
        }
    }

    std::ifstream file1(argv[1]);
    std::ifstream file2(argv[2]);
    std::ifstream file3(argv[3]);

    const json db1 = json::parse(file1);
    const json db2 = json::parse(file2);
    const json db3 = json::parse(file3);
    json dbs[] = {db1, db2, db3};
    

    double throughput_mbps_arr[3];
    double packetsPerSecond_arr[3];
    //double packetLoss_arr[3];
    double throughputLoss_arr[3];
    double packetsPerSecondLoss_arr[3];

    std::cout << "\n\n" << std::endl;

    for (int i = 0; i < 3; i++) {
        const auto& single_object = dbs[i][0];

        // Bits per second from address to address
        const std::string AtoB_bps_str = single_object["Bits/s A → B"];
        const std::string BtoA_bps_str = single_object["Bits/s B → A"];            
        const double AtoB_bps = std::stod(AtoB_bps_str);
        const double BtoA_bps = std::stod(BtoA_bps_str);

        // Packets
        const std::string totalPackets_str = single_object["Total Packets"];
        const int totalPackets = std::stoi(totalPackets_str);

        // Duration and Throughput
        const std::string duration_str = single_object["Duration"];
        const double duration = std::stod(duration_str);
        const double avg_stream_throughput_bps = AtoB_bps + BtoA_bps;
        const double avg_stream_throughput_mbps = std::round((avg_stream_throughput_bps / 1000000) * 1000000.0) / 1000000.0;
            
        // Packets per second
        // Expected packet rate should be the average PPS out of 5 baseline runs
        // Sometimes there might be a negative value, so we have a tolerance of 3% or .03
        const double packetsPerSecond = std::round(totalPackets / duration);
        const double Expected = 234.333;
        // const double packetLoss = 
        //    std::max(0.0, ((Expected - packetsPerSecond) / Expected) - 0.03) * 100;
        const double throughput_loss = (1.0 - (avg_stream_throughput_mbps / 0.388461)) * 100.0;
        const double pps_loss = (1.0 - (packetsPerSecond / Expected)) * 100;

        // Plugging in calculations into each array, later calculating of all the elements
        throughput_mbps_arr[i] = avg_stream_throughput_mbps;
        packetsPerSecond_arr[i] = packetsPerSecond;
        //packetLoss_arr[i] = packetLoss;
        throughputLoss_arr[i] = throughput_loss;
        packetsPerSecondLoss_arr[i] = pps_loss;


        // Displaying statistics
        std::cout << "Run #" << i + 1 << ":\n"
                  << "Total Packets: " << totalPackets << "\n"
                  << "Wireshark Capture Duration: " << duration << "s\n"
                  << "Bits/s A → B: " << AtoB_bps << " bps\n"
                  << "Bits/s B → A: " << BtoA_bps << " bps\n\n"
                  << "Average Throughput: " << avg_stream_throughput_bps << " bps   |   "
                                          << avg_stream_throughput_mbps << " Mbps" << "\n"
                  << "Packets per second: " << packetsPerSecond << "\n"
                  << "Throughput Loss: " << throughput_loss << "%\n"
                  << "PPS Loss: " << pps_loss << "%\n\n"
                  << "----------------------------------------\n\n";
    }

        
    std::cout << "\nAverage of all runs:\n"
              << "Average Throughput: " << mean(throughput_mbps_arr) << " Mbps" << "\n"
              << "Packets per second: " << mean(packetsPerSecond_arr) << "\n"
              << "Throughput Change: " << mean(throughputLoss_arr) << "%\n"
              << "PPS Change: " << mean(packetsPerSecondLoss_arr) << "%\n\n";

    return 0;
}
