#ifndef CLVHDMASTER_H
#define CLVHDMASTER_H

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

#include "clvHdEMG.hpp"   // EMG class
#include "com_client.hpp" // COM class
#include "registers.hpp"  // Registers class

#define GLOBAL_VERBOSE_LEVEL 0

namespace ClvHd
{
class EMG;

/**
 * @brief The CleverHand Master board class
 *
 * This class is the main class of the library.
 * It is used to communicate with the master board and to read/write registers of each modules.
 *
 * @author Alexis Devillard
 * @date 2022
 */
class Master : public Communication::Client
{
    using clk = std::chrono::system_clock;
    using sec = std::chrono::duration<double>;

    public:
    Master(int verbose = -1) : ESC::CLI(verbose, "ClvHd-Master"){};
    ~Master();

    void
    printBit(int8_t val);

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
    readReg(uint8_t id, uint8_t reg, size_t size, const void *buff);

    int8_t
    readReg8(uint8_t id, uint8_t reg);
    int16_t
    readReg16(uint8_t id, uint8_t reg);
    int32_t
    readReg32(uint8_t id, uint8_t reg);
    int64_t
    readReg64(uint8_t id, uint8_t reg);
    float
    readRegFloat(uint8_t id, uint8_t reg);
    double
    readRegDouble(uint8_t id, uint8_t reg);

  void serial_connection(std::string path)
  {
    this->open_connection(Communication::Client::Mode::SERIAL, path.c_str());
  };

  /**
   * @brief writeReg write one byte to reg address to the module with the given id.
   *
   * @param id Id of the module to write to.
   * @param reg Start address of the data to write.
   * @param val Value to write.
   * @return int Number of bytes written.
   */
  int
  writeReg(uint8_t id, uint8_t reg, char val);

  /**
   * @brief Get the number of modules connected to the master board.
   * @return The number of modules connected to the master board.
   */
  int
  getNbModules()
  {
      char msg[6] = {'n', 0, 0, 0};
      writeS(msg, 4, true);
      int n = readS((uint8_t *)msg, 4, true);
      if(n == 4)
          return msg[0];
      else
          return -1;
  };

  /**
   * @brief Check if the connection to the master board is established
   * @return True if the connection is established, false otherwise.
   */
  bool
  test_connection()
  {
      char msg[6] = {'m', 1, 2, 3};
      char ans[6];
      writeS(msg, 4, true);
      int n = readS((uint8_t *)ans, 6, true);
      // logln(std::to_string(n) + " [ " + std::to_string((int)ans[0]) + " " +
      //           std::to_string((int)ans[1]) + " " +
      //           std::to_string((int)ans[2]) + " " +
      //           std::to_string((int)ans[3]) + " ]",
      //       true);
      if(n != 6)
          return false;
      else
          for(int i = 0; i < 4; i++)
              if(ans[i] != msg[i])
                  return false;
      return true;
  };

  /**
   * @brief Trigger a blinking effect on the module with the given id.
   * @param id Id of the module to blink.
   * @param dt_cs Duration of the blinking in centiseconds
   * @param nb_blink Number of blinking.
   */
  void
  blink(uint8_t id, uint8_t dt_cs, uint8_t nb_blink)
  {
      uint8_t msg[6] = {'b', id, dt_cs, nb_blink};
      writeS(msg, 4, true);
  };

  /**
   * @brief Get the version of the master board.
   * @param i Index of the version value. For i = 1, the major version is returned. For i = 2, the minor version is returned. For i = 0, a short composed of the major and minor version is returned.
   * @return The version of the master board. If 0 is returned, an error occured.
   */
  uint16_t
  getVersion(int i)
  {
      char msg[6] = {'v', 0, 0, 0};
      writeS(msg, 4, true);
      if(readS((uint8_t *)msg, 4, true) == 4)
      {
          if(i == 0)
              return *(uint16_t *)(msg);
          if(i == 1)
              return msg[0];
          if(i == 2)
              return msg[1];
      }
      return 0;
  };

  /**
   * @brief Get the version of the master board.
   * @return The version of the master board as a std::string. If an empty string is returned, an error occured.
   */
  std::string
  getVersion()
  {
      char msg[6] = {'v', 0, 0, 0};
      writeS(msg, 4, true);
      if(readS((uint8_t *)msg, 4, true) == 4)
          return std::to_string((int)msg[0]) + "." +
                 std::to_string((int)msg[1]);
      else
          return "";
  };

  /**
   * @brief setup Get the number of EMG modules connected to the master board.
   * @return int The number of EMG modules connected to the master board.
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
  void
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
   * @brief read_fast_data Read (actual request to the master board) the fast data(2byte) from the given channel of the module with the given id.
   *
   * @param id Id of the module to read from.
   * @param channel Channel to read from.
   * @return double Converted value of the fast data. Unit is mV.
   */
  double
  read_fast_EMG(int id, int channel);

  /**
   * @brief read_precise_data Read (actual request to the master board) the precise data(3byte) from the given channel of the module with the given id.
   *
   * @param id Id of the module to read from.
   * @param channel Channel to read from.
   * @return double Converted value of the precise data. Unit is mV.
   */
  double
  read_precise_EMG(int id, int channel);

  /**
   * @brief fast_EMG Read the previously requested fast data(2byte) from the given channel of the module with the given id.
   *
   * @param id Id of the module to read from.
   * @param channel Channel to read from.
   * @return double Converted value of the fast data. Unit is mV.
   */
  double
  fast_EMG(int id, int channel);

  /**
   * @brief precise_EMG Read the previously requested precise data(3byte) from the given channel of the module with the given id.
   *
   * @param id Id of the module to read from.
   * @param channel Channel to read from.
   * @return double Converted value of the precise data. Unit is mV.
   */
  double
  precise_EMG(int id, int channel);

  /**
   * @brief Set all modules to converstion state.
   */
  void
  start_acquisition();

  /**
   * @brief Set all modules to standby state.
   */
  void
  stop_acquisition();

  /**
   * @brief Read all data registers (State, 3fast, 3precise) from each available module.
   */
  int
  read_all_signal();

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
      return "Master board: " + std::to_string(m_EMG.size()) +
             " EMG module(s) connected.";
  };

  std::string
  repr() const
  {
      return "Master object";
  };

  EMG &
  emg(int i)
  {
      return m_EMG[i];
  };

  std::vector<EMG> m_EMG;

  private:
  bool m_streaming = false;
  ADS1293_Reg m_streaming_reg;
  size_t m_streaming_size;
  std::vector<std::pair<int, uint8_t>> m_streaming_channels;
  uint8_t m_buffer[512];
};
} // namespace ClvHd
#endif
