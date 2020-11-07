/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/statusexception.h"

#include <QReadLocker>
#include <QString>
#include <QWriteLocker>

namespace BlackMisc
{
    CStatusException::CStatusException(const CStatusMessage &payload) :
        m_payload(payload)
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
} // ns
