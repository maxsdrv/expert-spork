#pragma once

#include "log/Log2.h"

#include <QByteArray>
#include <QtEndian>

#include <limits>
#include <vector>

namespace rpc
{

// [Sync + Checksum + Length] Big endian
template <typename Sync = quint16, typename Length = quint32>
class SyncChecksumLengthBEHeader
{
public:
    // NOLINTNEXTLINE (hicpp-explicit-conversions)
    SyncChecksumLengthBEHeader(Sync sync) : m_sync(qToBigEndian(sync))
    {
    }

    QByteArray singleWrap(const QByteArray& array) const noexcept
    {
        if (static_cast<quint64>(array.size())
            > static_cast<quint64>(std::numeric_limits<Length>::max()))
        {
            LOG_WARNING_CH("rpc") << "Rpc message size overflow";
            return {};
        }

        const Data data{
            .sync = m_sync,
            .summ = qToBigEndian(qChecksum(array.constData(), array.size())),
            .length = qToBigEndian(static_cast<Length>(array.size()))};

        QByteArray message(reinterpret_cast<const char*>(&data), sizeof(data));
        return message + array;
    }

    std::pair<QByteArray, QByteArray> singleUnwrap(const QByteArray& array) const noexcept
    {
        LOG_TRACE_CH("rpc") << "Single unwrap enter: " << array.size() << " bytes";

        if (array.size() < static_cast<int>(sizeof(Data)))
        {
            LOG_TRACE_CH("rpc") << "Not enough data for header";
            return {{}, array};
        }

        auto sync = QByteArray(reinterpret_cast<const char*>(&m_sync), sizeof(Sync));
        auto syncIndex = array.indexOf(sync);
        if (syncIndex == -1)
        {
            // No sync found, trash all data
            LOG_TRACE_CH("rpc") << "No sync found";
            return {{}, {}};
        }

        auto chunk = array.mid(syncIndex);

        auto* data = reinterpret_cast<const Data*>(chunk.constData());
        auto summ = qFromBigEndian(data->summ);
        auto length = qFromBigEndian(data->length);

        if (chunk.length() < static_cast<int>(length + sizeof(Data)))
        {
            // Not enough data, put chunk to remainder
            LOG_TRACE_CH("rpc") << "Not enough data for message";
            return {{}, chunk};
        }

        auto message = chunk.mid(sizeof(Data), length);
        if (summ != qChecksum(message.constData(), message.length()))
        {
            // Checksum failed, skip sync and try again
            LOG_WARNING_CH("rpc") << "Rpc message header checksum failure";
            return singleUnwrap(chunk.mid(sizeof(Sync)));
        }

        // All ok, return message and remainder
        LOG_TRACE_CH("rpc") << "Message unwrapped: " << message.size() << " bytes";
        return {message, chunk.right(chunk.size() - message.size() - sizeof(Data))};
    }

private:
    struct Data
    {
        Sync sync;
        quint16 summ;
        Length length;
    };

    Sync m_sync;
};

template <typename Header>
class HeaderMessageWrapper
{
public:
    explicit HeaderMessageWrapper(Header header) : m_header(std::move(header))
    {
    }

    QByteArray wrap(const QByteArray& array) noexcept
    {
        return m_header.singleWrap(array);
    }

    std::vector<QByteArray> unwrap(const QByteArray& array)
    {
        m_unwrapData += array;
        std::vector<QByteArray> unwrapped;

        for (;;)
        {
            auto [message, remainder] = m_header.singleUnwrap(m_unwrapData);
            m_unwrapData = remainder;
            if (!message.isEmpty())
            {
                unwrapped.push_back(message);
            }
            if (message.isEmpty() || remainder.isEmpty())
            {
                break;
            }
        }

        return unwrapped;
    }

private:
    Header m_header;
    QByteArray m_unwrapData;
};

} // namespace rpc
