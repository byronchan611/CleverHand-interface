#include "clvHdController.hpp"
#include <iomanip>
#include <iostream>
#include <lsl_cpp.h>
#include <stdio.h>
#include <stdlib.h>


int
main()
{
    std::cout << "CleverHand Serial Interface:" << std::endl;
    try
    {
        ClvHd::Controller controller(3);
        controller.open_connection("/dev/ttyACM0", 500000, O_RDWR | O_NOCTTY);
        usleep(500000);
        controller.setup();
	    std::cout << "CleverHand Serial Interface:" << std::endl;

        //setup the emg modules
        int route_table[3][2] = {{0, 1}, {0, 1}, {0, 1}};
        bool chx_enable[3] = {true, false, false};
        bool chx_high_res[3] = {true, true, true};
        bool chx_high_freq[3] = {true, true, true};
        int R1[3] = {4, 4, 4};
        int R2 = 4;
        int R3[3] = {4, 4, 4};
        controller.setupEMG(0, route_table, chx_enable, chx_high_res, chx_high_freq, R1, R2, R3);
        //start the emg modules
        controller.start_acquisition();
        std::vector<double> sample(6);
        for(int t = 0;; t++)
        {
            uint64_t timestamp;
            controller.read_all_signal(&timestamp);
            for(int i = 0; i < 3; i++)
            {
                sample[i] = controller.fast_EMG(0, i);
                sample[i + 3] = controller.precise_EMG(0, i);
            }
            std::cout << "timestamp: " << timestamp << " ";
            for(int i = 0; i < 6; i++)
                std::cout << std::fixed << std::setprecision(2) << sample[i] << " ";
            std::cout << std::endl;
            usleep(1000);
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
