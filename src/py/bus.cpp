#include <boost/python.hpp>
#include <cctalk/bus.hpp>
#include <memory>

namespace cctalk {
    boost::asio::io_context ioContext;


    class PyBus: public Bus {
    public:
        PyBus(): Bus(ioContext) {}
    };
}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(send1_overloads, send, 1, 2);

BOOST_PYTHON_MODULE(cctalk_py) {
    using boost::python::class_;
    using boost::python::enum_;
    using boost::python::no_init;
    using boost::python::init;
    using cctalk::PyBus;

    typedef void (PyBus::*send1)(const PyBus::Command, std::function<void (bool)>);
    typedef void (PyBus::*send2)(const PyBus::DataCommand, std::function<void (bool)>);

    enum_<PyBus::HeaderCode>("HeaderCode")
        .value("resetDevice", PyBus::RESET_DEVICE)
        .value("requestCoinId", PyBus::REQUEST_COIN_ID)
        .value("requestAcceptCounter", PyBus::REQUEST_ACCEPT_COUNTER)
        .value("modifyMasterInhibitState", PyBus::MODIFY_MASTER_INHIBIT_STATE)
        .value("readBufferedCreditOrErrorCodes", PyBus::READ_BUFFERED_CREDIT_OR_ERROR_CODES)
        .value("modifyInhibitStatus", PyBus::MODIFY_INHIBIT_STATUS)
        .value("performSelfTest", PyBus::PERFORM_SELF_TEST)
        .value("readInputLines", PyBus::READ_INPUT_LINES)
        .value("requestSoftwareVersion", PyBus::REQUEST_SOFTWARE_VERSION)
        .value("requestSerialNumber", PyBus::REQUEST_SERIAL_NUMBER)
        .value("requestProductCode", PyBus::REQUEST_PRODUCT_CODE)
        .value("requestEquipmentCategoryId", PyBus::REQUEST_EQUIPMENT_CATEGORY_ID)
        .value("requestStatus", PyBus::REQUEST_STATUS)
        .value("simplePoll", PyBus::SIMPLE_POLL);


    class_<PyBus::Command>("Command")
        .def_readwrite("destination", &PyBus::Command::destination)
        .def_readwrite("source", &PyBus::Command::destination)
        .def_readwrite("header", &PyBus::Command::header);

    class_<PyBus::DataCommand>("DataCommand")
        .def_readwrite("destination", &PyBus::DataCommand::destination)
        .def_readwrite("source", &PyBus::DataCommand::destination)
        .def_readwrite("header", &PyBus::DataCommand::header)
        .def_readwrite("data", &PyBus::DataCommand::data)
        .def_readwrite("length", &PyBus::DataCommand::length);

    class_<PyBus, boost::noncopyable>("Bus")
        .def("open", &PyBus::open)
        .def("ready", &PyBus::ready)
        .def("send", (send1)&PyBus::send, send1_overloads())
        .def("receive", &PyBus::receive);
}
