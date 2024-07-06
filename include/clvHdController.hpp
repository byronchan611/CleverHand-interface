#ifndef CLVHDCONTROLLER_H
#define CLVHDCONTROLLER_H

#include <cmath>
#include <cstring>
#include <iostream> // std::cout, std::endl
#include <string>
#include <vector>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include <stdint.h> // uint8_t, uint16_t, uint32_t, uint64_t
#include <stdio.h>  // Standard input/output definitions

#include "clvHdEMG.hpp"  // EMG class
#include "registers.hpp" // Registers class
#include <serial_client.hpp>

namespace ClvHd
{
class EMG;

/**
 * @brief The CleverHand Controller board class
 *
 * This class is the main class of the library.
 * It is used to communicate with the controller board and to read/write registers of each modules.
 *
 * @author Alexis Devillard
 * @date 2022
 */
class Controller : public Communication::Serial
{
    using clk = std::chrono::system_clock;
    using sec = std::chrono::duration<double>;

    public:
    Controller(int verbose = -1)
        : Communication::Serial(verbose), ESC::CLI(verbose, "ClvHd-Controller"){};
    ~Controller();

    /**
     * @brief readReply Read a reply from the controller board. The reply contains a timestamp, a size and the data.
     *
     * @param buff Buffer to store the data.
     * @param timestamp Timestamp of the reply.
     * @return int Number of bytes read.
     */
    int
    readReply(uint8_t *buff, uint64_t *timestamp = nullptr)
    {
        //logln("Reading reply", true);
        int n = this->readS(m_buffer, 9);
        // logln("Read: " + std::to_string(n) + " bytes", true);
        // logln("timestamp: " + std::to_string(*(uint64_t *)m_buffer) +
        //           " size: " + std::to_string(m_buffer[8]),
        //       true);
        if(n == 9)
        {
            if(timestamp != nullptr)
                *timestamp = *(uint64_t *)m_buffer;
            n = this->readS(buff, m_buffer[8]);
            return n;
        }
        else
            return -1;
    };

    /**
     * @brief sendCmd Send a command to the controller board.
     *
     * @param cmd Command to send.
     * @param id Id of the module to send the command to.
     * @param data Data to send.
     * @param size Size of the data to send.
     * @return int 0 if the command was sent successfully, -1 otherwise.
     */
    int
    sendCmd(uint8_t cmd,
            uint8_t id = 0,
            uint8_t *data = nullptr,
            size_t size = 0)
    {
        uint8_t msg[m_packet_size] = {cmd, id};
        for(int i = 0; i < size; i++) msg[i + 2] = data[i];
        int n = this->writeS(msg, m_packet_size);
        // log("Sent: [cmd: " + std::to_string(cmd) + " id: " + std::to_string(id) +
        //         " data: ",
        //     true);
        // for(int i = 0; i < size; i++) log(std::to_string(data[i]) + " ", false);
        // log("]\n", false);
        if(n != m_packet_size)
        {
            logln("Error sending command", true);
            return -1;
        }
        return size;
    };

    /**
     * @brief readReg read size byte starting from reg address from the module with the given id.
     *
     * @param id Id of the module to read from.
     * @param reg Start address of the data to read.
     * @param size Number of bytes to read.
     * @param buff Buffer to store the data.
     * @return int Number of bytes read.
     */
    int
    readReg(uint8_t id,
            uint8_t reg,
            uint8_t size,
            const void *buff,
            uint64_t *timestamp = nullptr);

    /**
     * @brief writeReg write one byte to reg address to the module with the given id.
     *
     * @param id Id of the module to write to.
     * @param reg Start address of the data to write.
     * @param val Value to write.
     * @return int Number of bytes written.
     */
    int
    writeReg(uint8_t id, uint8_t reg, uint8_t val);

    /**
     * @brief Get the number of modules connected to the controller board.
     * @return The number of modules connected to the controller board.
     */
    int
    getNbModules()
    {
        sendCmd('n');
        uint8_t nb_modules;
        if(readReply(&nb_modules) == sizeof(nb_modules))
            return nb_modules;
        else
            return -1;
    };

    /**
     * @brief Check if the connection to the controller board is established
     * @return True if the connection is established, false otherwise.
     */
    bool
    test_connection()
    {
        uint8_t arr[3] = {1, 2, 3};
        int n = sendCmd('m', 0, arr, 3);
        uint8_t ans[3];
        if(readReply(ans) == 3)
            return ans[0] == ans[0] && ans[1] == arr[1] && ans[2] == arr[2];
        else
            return false;
    };

    /**
     * @brief Get the version of the controller board.
     * @return The version of the controller board as a std::string. If an empty string is returned, an error occured.
     */
    std::string
    getVersion(uint8_t *major = nullptr, uint8_t *minor = nullptr)
    {
        sendCmd('v');
        uint8_t ans[2];
        if(readReply(ans) == 2)
        {
            if(major != nullptr)
                *major = ans[0];
            if(minor != nullptr)
                *minor = ans[1];
            return std::to_string((int)ans[0]) + "." +
                   std::to_string((int)ans[1]);
        }
        else
            return "";
    };

    /**
     * @brief setup Get the number of EMG modules connected to the controller board.
     * @return int The number of EMG modules connected to the controller board.
     */
    int
    setup();

    /**
     * @brief setupEMG Setup the EMG module with the given id.
     *
     * @param n_board Id of the EMG module to setup.
     * @param route_table Route table of the EMG module.
     * @param chx_enable Enable/disable of the EMG module.
     * @param chx_high_res High resolution of the EMG module.
     * @param chx_high_freq High frequency of the EMG module.
     * @param R1 R1 of the EMG module.
     * @param R2 R2 of the EMG module.
     * @param R3 R3 of the EMG module.
     */
    int
    setupEMG(int n_board,
             int route_table[3][2],
             bool chx_enable[3],
             bool chx_high_res[3],
             bool chx_high_freq[3],
             int R1[3],
             int R2,
             int R3[3]);

    /**
     * @brief data_ready Check if the data is ready to be read from from the given channel of the module with the given id.
     *
     * @param id Id of the module to check.
     * @param channel Channel to check.
     * @param precise If true, the function will return true if the precise signal (3bytes) of the given channel is ready to be read. If false, the function will return true if the fast signal (2byte) of the given channel is ready to be read.
     * @return true
     * @return false
     */
    bool
    data_ready(int id, int channel, bool precise = false);

    /**
     * @brief read_fast_data Read (actual request to the controller board) the fast data(2byte) from the given channel of the module with the given id.
     *
     * @param id Id of the module to read from.
     * @param channel Channel to read from.
     * @return double Converted value of the fast data. Unit is mV.
     */
    double
    read_fast_EMG(int id, int channel, bool converted = true);

    /**
     * @brief read_precise_data Read (actual request to the controller board) the precise data(3byte) from the given channel of the module with the given id.
     *
     * @param id Id of the module to read from.
     * @param channel Channel to read from.
     * @return double Converted value of the precise data. Unit is mV.
     */
    double
    read_precise_EMG(int id, int channel, bool converted = true);

    /**
     * @brief fast_EMG Read the previously requested fast data(2byte) from the given channel of the module with the given id.
     *
     * @param id Id of the module to read from.
     * @param channel Channel to read from.
     * @return double Converted value of the fast data. Unit is mV.
     */
    double
    fast_EMG(int id, int channel, bool converted = true);

    /**
     * @brief precise_EMG Read the previously requested precise data(3byte) from the given channel of the module with the given id.
     *
     * @param id Id of the module to read from.
     * @param channel Channel to read from.
     * @return double Converted value of the precise data. Unit is mV.
     */
    double
    precise_EMG(int id, int channel, bool converted = true);

    /**
     * @brief Set all modules to converstion state.
     */
    int
    start_acquisition();

    /**
     * @brief Set all modules to standby state.
     */
    int
    stop_acquisition();

    /**
     * @brief Read all data registers (State, 3fast, 3precise) from each available module.
     */
    int
    read_all_signal(uint64_t *timestamp = nullptr);

    /**
     * @brief get_error Get the active error from the module with the given id.
     *
     * @param id Id of the module to read from.
     * @param verbose If true, the function will return a more detailed error message.
     * @return std::string A list of the active error.
     */
    std::string
    get_error(int id, bool verbose = false);

    bool
    error_at(int id, int index);

    operator std::string() const
    {
        return "Controller board: " + std::to_string(m_EMG.size()) +
               " EMG module(s) connected.";
    };

    std::string
    repr() const
    {
        return "Controller object";
    };

    EMG &
    emg(int i)
    {
        return *m_EMG[i];
    };

    std::vector<EMG *> m_EMG;

    private:
    bool m_streaming = false;
    ADS1293_Reg m_streaming_reg;
    size_t m_streaming_size;
    std::vector<std::pair<int, uint8_t>> m_streaming_channels;
    uint8_t m_buffer[512];
    int m_packet_size = 6;
};
} // namespace ClvHd
#endif
