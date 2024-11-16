// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/statusexception.h"

#include <QReadLocker>
#include <QWriteLocker>

namespace swift::misc
{
    CStatusException::CStatusException(const CStatusMessage &payload) : m_payload(payload)
    {}

    CStatusException::CStatusException(const CStatusException &other) : std::exception(other), m_payload(other.m_payload)
    {
        QReadLocker lock(&other.m_lock);
        this->m_temp = other.m_temp;
    }

    const char *CStatusException::what() const noexcept
    {
        QWriteLocker lock(&this->m_lock);
        if (m_temp.isNull()) { m_temp = m_payload.getMessage().toLocal8Bit(); }
        return m_temp;
    }

    void CStatusException::maybeThrow(const CStatusMessage &message)
    {
        if (!message.isEmpty()) { throw CStatusException(message); }
    }
} // namespace swift::misc
