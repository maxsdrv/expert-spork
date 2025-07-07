#pragma once

#include <QObject>

#include "gsl/pointers"

namespace mpk::dss::core
{

class TargetController;

class TargetNotifier : public QObject
{
    Q_OBJECT

public:
    explicit TargetNotifier(
        gsl::not_null<TargetController*> controller, QObject* parent = nullptr);

signals:
    void notify(QString, QJsonObject);

private:
    gsl::not_null<TargetController*> m_controller;
};

} // namespace mpk::dss::core
