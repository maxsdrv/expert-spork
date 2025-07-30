#pragma once

#include "UserPermissionsProvider.h"

#include <QMap>

namespace mpk::rest
{

class UserPermissionsStatic : public UserPermissionsProvider
{
public:
    using GroupsMap = QMap<http::Permissions::Group, QStringList>;
    explicit UserPermissionsStatic(GroupsMap groups);

    http::Permissions list(const QString& username = QString()) const override;

private:
    GroupsMap m_groups;
};

} // namespace mpk::rest
