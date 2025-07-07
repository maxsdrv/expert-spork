#pragma once

#include "Command.h"

#include <QObject>

#include "gsl/pointers"

namespace mpk::dss::backend::tamerlan
{

class ModbusCommandExecutor;

class CommandBatch : public QObject
{
    Q_OBJECT

public:
    using Commands = std::vector<Command>;

public:
    explicit CommandBatch(Commands commands = {}, QObject* parent = nullptr);

    [[nodiscard]] Commands commands() const;

public:
    void execute(gsl::not_null<ModbusCommandExecutor*> executor);
    [[nodiscard]] bool running() const;

signals:
    void succeeded();
    void failed(QString);
    void finished();

private:
    Commands m_commands;
    bool m_running = false;
};

} // namespace mpk::dss::backend::tamerlan
