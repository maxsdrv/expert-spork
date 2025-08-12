#pragma once

#include "Common.h"

#include "gsl/assert"

#include <array>
#include <span>

namespace mpk::drone::core
{

static constexpr auto defaultChunkSize = (maxModbusWriteBytes / 4) * 4 - 4;

template <int chunkSize = defaultChunkSize>
requires(
    chunkSize % 4 == 0 && chunkSize + 4 <= maxModbusWriteBytes) struct FileChunk
{
    std::uint16_t size = 0;
    std::array<unsigned char, chunkSize> bytes = {};

    static void toRegs(const FileChunk& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        *it = data.size & sixteenBitMask;

        // TODO: works properly only on even number of bytes
        auto odd = false;
        auto reg = 0U;
        for (const auto& byte: data.bytes)
        {
            if (!odd)
            {
                reg = static_cast<unsigned int>(byte);
            }
            else
            {
                reg |= static_cast<unsigned int>(byte) << eightBit;
                *it = static_cast<std::uint16_t>(reg);
            }
            odd = !odd;
        }
    }

    static FileChunk fromRegs(U16Iterator auto begin, U16Iterator auto end)
    {
        Expects(std::distance(begin, end) == sizeof(FileChunk) / 2);

        FileChunk chunk;

        auto it = begin;
        chunk.size = static_cast<unsigned int>(*(it++));

        auto i = 0;
        auto bytes = std::span<unsigned char>(chunk.bytes);
        while (it != end)
        {
            bytes[i++] += *it & eightBitMask;
            bytes[i++] += *it >> eightBit;
            it++;
        }
        return chunk;
    }
};

} // namespace mpk::drone::core
