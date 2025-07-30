#pragma once

#include <yaml-cpp/yaml.h>

#include <QString>

namespace YAML
{

/**
 * Read yaml node from file (in YAML format)
 */
Node LoadFile(const QString& filename);

/**
 * Write yaml node to file (in YAML format)
 */
void SaveFile(const Node& node, const QString& filename);

/**
 * Write yaml emitter to file (in YAML format)
 */
void SaveFile(const Emitter& emitter, const QString& filename);

} // namespace YAML
