#include "dss-tamerlan-imitator/Paths.h"

#include "ApplicationFactory.h"
#include "ModbusServerController.h"

#include "log/Log2.h"

#include <QCoreApplication>

#include "gsl/assert"

int main(int argc, char* argv[])
{
    using namespace mpk::dss::imitator::tamerlan;

    QCoreApplication application(argc, argv);

    std::unique_ptr<ApplicationFactory> factory;
    try
    {
        factory = std::make_unique<ApplicationFactory>(argc, argv);
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << ex.what();
        return EXIT_FAILURE;
    }
    QObject::connect(
        factory.get(), &ApplicationFactory::exit, []() { exit(EXIT_SUCCESS); });

    std::unique_ptr<ModbusServerController> modbusServerController;
    try
    {
        modbusServerController = factory->createModbusServerController();
        Ensures(modbusServerController != nullptr);
        factory.reset(nullptr);
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << "An error occurred while creating modbus server: "
                  << ex.what();
        return EXIT_FAILURE;
    }

    LOG_INFO << "tamerlan imitator started";

    QObject::connect(
        &application,
        &QCoreApplication::aboutToQuit,
        modbusServerController.get(),
        &ModbusServerController::stop);

    QMetaObject::invokeMethod(
        modbusServerController.get(), "start", Qt::QueuedConnection);

    try
    {
        return QCoreApplication::exec();
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR
            << "Unhandled exception in application main loop: " << ex.what()
            << ". Exiting";
        return EXIT_FAILURE;
    }
    catch (...)
    {
        LOG_ERROR << "Unhandled exception in application main loop. Exiting";
        return EXIT_FAILURE;
    }
}
