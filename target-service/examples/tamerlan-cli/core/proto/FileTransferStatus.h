#pragma once

#include <cstdint>
#include <ostream>

namespace mpk::drone::core
{

enum class FileTransferStatus : std::uint16_t
{
    CLR = 0, // Clear, default state
    RTS,     // Ready to send
    RTR,     // Ready to receive
    CHW,     // Chunk written
    CHR,     // Chunk read
    EOT,     // End of transfer
    ERR,     // Error occurred
};

std::ostream& operator<<(std::ostream& stream, FileTransferStatus status);

} // namespace mpk::drone::core
