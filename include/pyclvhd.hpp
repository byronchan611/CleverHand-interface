#ifndef __PY_CLV_HD_HPP__
#define __PY_CLV_HD_HPP__

#include "clvHdEMG.hpp"
#include "clvHdController.hpp"
#include <boost/python.hpp>

using namespace boost::python;

//wrapper to access list
void
setupEMG(ClvHd::Controller &m,
         int n_board,
         boost::python::list _route_table,
         boost::python::list _chx_enable,
         boost::python::list _chx_high_res,
         boost::python::list _chx_high_freq,
         boost::python::list _R1,
         int R2,
         boost::python::list _R3)
{
    int route_table[3][2] = {
        {extract<int>(_route_table[0][0]), extract<int>(_route_table[0][1])},
        {extract<int>(_route_table[1][0]), extract<int>(_route_table[1][1])},
        {extract<int>(_route_table[2][0]), extract<int>(_route_table[2][1])}};
    bool chx_enable[3] = {extract<bool>(_chx_enable[0]),
                          extract<bool>(_chx_enable[1]),
                          extract<bool>(_chx_enable[2])};
    bool chx_high_res[3] = {extract<bool>(_chx_high_res[0]),
                            extract<bool>(_chx_high_res[1]),
                            extract<bool>(_chx_high_res[2])};
    bool chx_high_freq[3] = {extract<bool>(_chx_high_freq[0]),
                             extract<bool>(_chx_high_freq[1]),
                             extract<bool>(_chx_high_freq[2])};
    int R1[3] = {extract<int>(_R1[0]), extract<int>(_R1[1]),
                 extract<int>(_R1[2])};
    int R3[3] = {extract<int>(_R3[0]), extract<int>(_R3[1]),
                 extract<int>(_R3[2])};
    m.setupEMG(n_board, route_table, chx_enable, chx_high_res, chx_high_freq,
               R1, R2, R3);
};


BOOST_PYTHON_MODULE(pyclvhd)
{
    uint16_t (ClvHd::Controller::*v1)(int) = &ClvHd::Controller::getVersion;
    std::string (ClvHd::Controller::*v2)() = &ClvHd::Controller::getVersion;

    class_<ClvHd::Controller>("Controller", init<int>(arg("verbose") = -1))
        .def("serial_connection", &ClvHd::Controller::serial_connection,
             args("path"))
        .def("setupEMG", setupEMG,
             args("n_board", "route_table", "chx_enable", "chx_high_res",
                  "chx_high_freq", "R1", "R2", "R3"),
             "Setup the EMG module with the given id.\n\n@param n_board Id of "
             "the EMG module to setup.\n@param route_table Route table of the "
             "EMG module.\n@param chx_enable Enable/disable of the EMG "
             "module.\n@param chx_high_res High resolution of the EMG "
             "module.\n@param chx_high_freq High frequency of the EMG "
             "module.\n@param R1 R1 of the EMG module.\n@param R2 R2 of the "
             "EMG module.\n@param R3 R3 of the EMG module.")
        .def("readReg8", &ClvHd::Controller::readReg8, args("id", "reg"),
             "Read 8-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readReg16", &ClvHd::Controller::readReg16, args("id", "reg"),
             "Read 16-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readReg32", &ClvHd::Controller::readReg32, args("id", "reg"),
             "Read 32-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readReg64", &ClvHd::Controller::readReg64, args("id", "reg"),
             "Read 64-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:return: Register value")
        .def("readRegFloat", &ClvHd::Controller::readRegFloat, args("id", "reg"),
             "Read 32-bit float register\n\n:param id: ID of the EMG "
             "module\n:param reg: Register address\n:return: Register value")
        .def("readRegDouble", &ClvHd::Controller::readRegDouble, args("id", "reg"),
             "Read 64-bit double register\n\n:param id: ID of the EMG "
             "module\n:param reg: Register address\n:return: Register value")
        .def("writeReg", &ClvHd::Controller::writeReg, args("id", "reg", "val"),
             "Write 8-bit register\n\n:param id: ID of the EMG module\n:param "
             "reg: Register address\n:param val: Value to write")
        .def("getNbModules", &ClvHd::Controller::getNbModules,
             "Get the number of modules connected to the controller board")
        .def("test_connection", &ClvHd::Controller::test_connection,
             "Test the connection to the controller board")
        .def("getVersion", v1, args("i"),
             "Get the version of the controller board\n\n:param i: Index of the "
             "version value. For i = 1, the major version is returned. For i = "
             "2, the minor version is returned. For i = 0, a short composed of "
             "the major and minor version is returned.\n:return: The version "
             "of the controller board. If 0 is returned, an error occured.")
        .def("getVersion", v2,
             "Get the version of the controller board\n\n:return: The version of "
             "the controller board as a std::string. If an empty string is "
             "returned, an error occured.")
        .def("blink", &ClvHd::Controller::blink, args("id", "duration", "nb"),
             "Blink the LED of the module with the given id\n\n:param id: ID "
             "of the EMG module\n:param duration: Duration of the blink in "
             "cs\n:param nb: Number of blink")
        .def("setup", &ClvHd::Controller::setup, "Setup the controller board")
        .def("setupEMG", &ClvHd::Controller::setupEMG,
             args("id", "route_table", "chx_enable", "chx_high_res",
                  "chx_high_freq", "R1", "R2", "R3"),
             "Setup the EMG module with the given id\n\n:param id: Id of the "
             "EMG module to setup\n:param route_table: Route table of the EMG "
             "module\n:param chx_enable: Enable/disable of the EMG "
             "module\n:param chx_high_res: High resolution of the EMG "
             "module\n:param chx_high_freq: High frequency of the EMG "
             "module\n:param R1: R1 of the EMG module\n:param R2: R2 of the "
             "EMG module\n:param R3: R3 of the EMG module")
        .def("data_ready", &ClvHd::Controller::data_ready,
             args("id", "channel", "precise"),
             "Check if the data is ready to be read from from the given "
             "channel of the module with the given id.\n\n:param id: Id of the "
             "module to check.\n:param channel: Channel to check.\n:param "
             "precise: If true, the function will return true if the precise "
             "signal (3bytes) of the given channel is ready to be read. If "
             "false, the function will return true if the fast signal (2byte) "
             "of the given channel is ready to be read.\n:return: true if the "
             "data is ready to be read, false otherwise.")
        .def("read_fast_EMG", &ClvHd::Controller::read_fast_EMG,
             args("id", "channel"),
             "read_fast_data Read (actual request to the controller board) the "
             "fast data(2byte) from the given channel of the module with the "
             "given id.\n\n:param id: Id of the module to read.\n:param "
             "channel: Channel to read.\n:return: The fast data(2byte) from "
             "the given channel of the module with the given id.")
        .def("read_precise_EMG", &ClvHd::Controller::read_precise_EMG,
             args("id", "channel"),
             "read_precise_data Read (actual request to the controller board) the "
             "precise data(3byte) from the given channel of the module with "
             "the given id.\n\n:param id: Id of the module to read.\n:param "
             "channel: Channel to read.\n:return: The precise data(3byte) "
             "from the given channel of the module with the given id.")
        .def("fast_EMG", &ClvHd::Controller::fast_EMG, args("id", "channel"),
             "fast_EMG Read the previously requested fast data(2byte) from the "
             "given channel of the module with the given id.\n\n:param id: Id "
             "of the module to read.\n:param channel: Channel to "
             "read.\n:return: The fast data(2byte) from the given channel of "
             "the module with the given id.")
        .def(
            "precise_EMG", &ClvHd::Controller::precise_EMG, args("id", "channel"),
            "precise_EMG Read the previously requested precise data(3byte) "
            "from the given channel of the module with the given id.\n\n:param "
            "id: Id of the module to read.\n:param channel: Channel to "
            "read.\n:return: The precise data(3byte) from the given channel "
            "of the module with the given id.")
        .def("start_acquisition", &ClvHd::Controller::start_acquisition,
             "Set the "
             "modules in acquisition mode")
        .def("stop_acquisition", &ClvHd::Controller::stop_acquisition,
             "Set the "
             "modules in standby mode")
        .def("read_all_signal", &ClvHd::Controller::read_all_signal,
             "Read all data registers (State, 3fast, 3precise) from each "
             "available module.")
        .def(
            "get_error", &ClvHd::Controller::get_error, args("id", "verbose"),
            "Get the active error from the module with the given id.\n\n:param "
            "id: Id of the module to read.\n:param verbose: If true, If true, "
            "the function will return a more detailed error message.\n:return: "
            "The active error from the module with the given id.")
        .def("__str__", &ClvHd::Controller::operator std::string)
        .def("__repr__", &ClvHd::Controller::repr);

    // double (ClvHd::EMG::*conv1)(uint16_t) = &ClvHd::EMG::conv;
    // double (ClvHd::EMG::*conv2)(int, int32_t) = &ClvHd::EMG::conv;
    // class_<ClvHd::EMG>("EMG", init<ClvHd::Controller*, int>())
    // .def("setup", &ClvHd::EMG::setup)
    // .def("route_channel", &ClvHd::EMG::route_channel)
    // .def_readonly("m_mode", &ClvHd::EMG::get_mode)
    // .def("config_clock", &ClvHd::EMG::config_clock)
    // .def("enable_channels", &ClvHd::EMG::enable_channels)
    // .def("enable_SDM", &ClvHd::EMG::enable_SDM)
    // .def("enable_INA", &ClvHd::EMG::enable_INA)
    // .def("config_frequence", &ClvHd::EMG::config_frequence)
    // .def("config_resolution", &ClvHd::EMG::config_resolution)
    // .def("config_R1", &ClvHd::EMG::config_R1)
    // .def("config_R2", &ClvHd::EMG::config_R2)
    // .def("config_R3", &ClvHd::EMG::config_R3)
    // .def("read_precise_value", &ClvHd::EMG::read_precise_value)
    // .def("read_fast_value", &ClvHd::EMG::read_fast_value)
    // .def("precise_value", &ClvHd::EMG::precise_value)
    // .def("fast_value", &ClvHd::EMG::fast_value)
    // .def("conv", conv1)
    // .def("conv", conv2)
    // .def("get_error", &ClvHd::EMG::get_error)
    // .def("error_range_str", &ClvHd::EMG::error_range_str)
    // .def("error_status_str", &ClvHd::EMG::error_status_str);
}

#endif /* __PY_CLV_HD_HPP__ */
