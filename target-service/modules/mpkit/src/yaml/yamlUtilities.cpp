#include "yaml/yamlUtilities.h"

#include <fstream>

namespace YAML
{

Node LoadFile(const QString& filename)
{
    return YAML::LoadFile(filename.toStdString());
}

void SaveFile(const Node& node, const QString& filename)
{
    std::ofstream file(filename.toStdString());
    if (file.good())
    {
        file << node;
    }
    else
    {
        throw BadFile({});
    }
}

void SaveFile(const Emitter& emitter, const QString& filename)
{
    std::ofstream file(filename.toStdString());
    if (file.good())
    {
        file << emitter.c_str();
    }
    else
    {
        throw BadFile({});
    }
}

} // namespace YAML
