#include "httpws/JsonRestUtils.h"

namespace json
{

QJsonObject toTypeObject(const QString& type)
{
    QJsonObject object;
    object.insert(QStringLiteral("type"), toValue(type));
    return object;
}

} // namespace json
