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
        master.setupEMG(0, route_table, chx_enable, chx_high_res, chx_high_freq, R1, R2, R3);

        int nb_ch = 6;
        lsl::stream_info info_sample("EMG", "sample", nb_ch, 0,
                                     lsl::cf_float32);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<float> sample(nb_ch);
        master.start_acquisition();
        std::cout << "[INFOS] Now sending data... " << std::endl;
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
            outlet_sample.push_sample(sample, timestamp/1000000.0);
        }
        // for(int t = 0;; t++)
        // {
        //     uint64_t timestamp;
        //     master.read_all_signal(&timestamp);
        //     // std::cout << "timestamp: " << timestamp << " ";
        //     // std::cout << (int)master.m_EMG[0]->get_regs()[ClvHd::DATA_STATUS_REG] << " 0b" << master.byte2bits(master.m_EMG[0]->get_regs()[ClvHd::DATA_STATUS_REG]) << std::endl;
        //     for(int i = 0; i < 3; i++)
        //     {
        //         sample[i] = master.fast_EMG(0, i);
        //         sample[i + 3] = master.precise_EMG(0, i);
        //     }
        //     outlet_sample.push_sample(sample, timestamp/1000000.0);
        //     std::cout << "t: " << std::setw(9) << timestamp << ", EMG: " << std::setw(7) << sample[0] << " " << std::setw(7) << sample[1] << " " << std::setw(7) << sample[2] << " " << std::setw(7) << sample[3] << " " << std::setw(7) << sample[4] << " " << std::setw(7) << sample[5] << std::endl;
        // }
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
