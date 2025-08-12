#pragma once

#include "core/proto/FileChunk.h"
#include "core/proto/FileTransferStatus.h"

#include <QFile>
#include <QObject>

#include "gsl/pointers"

#include <iostream>

namespace mpk::drone::core
{

class ModbusData;

class ModbusFileTransfer : public QObject
{
    Q_OBJECT

public:
    ModbusFileTransfer(
        const QString& filename,
        gsl::not_null<ModbusData*> modbusData,
        QObject* parent = nullptr);

signals:
    void fileReceived(QString);

private slots:
    void onStatusChanged(FileTransferStatus);

private:
    void setStatus(FileTransferStatus status);

private:
    QFile m_file;
    FileChunk<defaultChunkSize> m_chunk;
    gsl::not_null<ModbusData*> m_modbusData;
};

} // namespace mpk::drone::core
