#include "clvHdMaster.hpp"
#include <iomanip>
#include <iostream>
#include <lsl_cpp.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    std::cout << "CleverHand Serial Interface:" << std::endl;
    try
    {
        ClvHd::Master master(3);
        master.open_connection("/dev/ttyACM0", 500000, O_RDWR | O_NOCTTY);
        usleep(500000);
        master.setup();
        std::cout << "CleverHand Serial Interface:" << std::endl;

        //setup the emg modules
        int route_table[3][2] = {{std::stoi(argv[1]), std::stoi(argv[2])}, {std::stoi(argv[3]), std::stoi(argv[4])}, {std::stoi(argv[5]), std::stoi(argv[6])}};
        bool chx_enable[3] = {true, true, true};
        bool chx_high_res[3] = {true, true, true};
        bool chx_high_freq[3] = {true, true, true};
        int R1[3] = {2, 2, 2};
        int R2 = 4;
        int R3[3] = {4, 4, 4};
        // std::string str = master.m_EMG[0]->dump_regs(true);
        // std::cout << str << std::endl;
        master.setupEMG(0, route_table, chx_enable, chx_high_res, chx_high_freq, R1, R2, R3);
        // //start the emg modules
        master.start_acquisition();
        // std::string str = master.m_EMG[0]->dump_regs();
        
        std::vector<double> sample(6);
        for(int t = 0;; t++)
        {
            uint64_t timestamp;
            master.read_all_signal(&timestamp);
            for(int i = 0; i < 3; i++)
            {
                uint32_t val = master.precise_EMG(0, i, false);
                val = (__builtin_bswap32(val) >> 8);
                sample[i] = val;
                uint16_t val2 = master.fast_EMG(0, i, false);
                val2 = __builtin_bswap16(val2);
                sample[i + 3] = val2;
            }
            printf("t: % 9ld, EMG: % 7.3f % 7.3f % 7.3f % 7.3f % 7.3f % 7.3f\n",
                   timestamp, sample[0], sample[1], sample[2], sample[3],
                   sample[4], sample[5]);
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
    catch(std::string str)
    {
        std::cerr << "[ERROR] Got an exception: " << str << std::endl;
    }

    return 0; // success
}
