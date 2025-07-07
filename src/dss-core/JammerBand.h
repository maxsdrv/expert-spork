#pragma once

#include <QMetaType>
#include <QObject>

#include <vector>

namespace mpk::dss::core
{

using JammerLabel = QString;

struct JammerBand
{
    JammerLabel label;
    bool active;
};

bool operator==(const JammerBand& left, const JammerBand& right);
bool operator!=(const JammerBand& left, const JammerBand& right);

using JammerBands = std::vector<JammerBand>;

JammerBands setAll(JammerBands bands, bool value);

} // namespace mpk::dss::core
