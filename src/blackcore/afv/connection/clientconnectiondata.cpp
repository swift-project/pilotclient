// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/afv/connection/clientconnectiondata.h"
#include "blackmisc/logmessage.h"
#include <QDebug>

using namespace BlackMisc;
using namespace BlackCore::Afv::Crypto;

namespace BlackCore::Afv::Connection
{
    const QStringList &CClientConnectionData::getLogCategories()
    {
        static const QStringList cats { CLogCategories::audio(), CLogCategories::vatsimSpecific() };
        return cats;
    }

    qint64 CClientConnectionData::secondsSinceAuthentication() const
    {
        return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc());
    }

    bool CClientConnectionData::isVoiceServerAlive() const
    {
        if (!m_lastVoiceServerHeartbeatAckUtc.isValid()) { return false; }
        const qint64 d = qAbs(m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()));
        return d < ServerTimeoutSecs;
    }

    bool CClientConnectionData::isDataServerAlive() const
    {
        if (!m_lastDataServerHeartbeatAckUtc.isValid()) { return false; }
        const qint64 d = qAbs(m_lastDataServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()));
        return d < ServerTimeoutSecs;
    }

    void CClientConnectionData::createCryptoChannels()
    {
        if (!m_tokens.isValid)
        {
            CLogMessage(this).warning(u"Tokens not set");
            return;
        }
        m_voiceCryptoChannel.reset(new CCryptoDtoChannel(m_tokens.VoiceServer.channelConfig));
    }

    void CClientConnectionData::setTsAuthenticatedToNow()
    {
        m_authenticatedDateTimeUtc = QDateTime::currentDateTimeUtc();
    }

    void CClientConnectionData::setTsHeartbeatToNow()
    {
        const QDateTime now = QDateTime::currentDateTimeUtc();
        m_lastVoiceServerHeartbeatAckUtc = now;
        m_lastDataServerHeartbeatAckUtc = now;
    }

    void CClientConnectionData::reset()
    {
        m_userName.clear();
        m_callsign.clear();
        m_authenticatedDateTimeUtc = QDateTime();
        m_lastVoiceServerHeartbeatAckUtc = QDateTime();
        this->setTokens({});
    }

    bool CClientConnectionData::voiceServerAlive() const
    {
        return (m_authenticatedDateTimeUtc.isValid() && timeSinceAuthenticationSecs() < ServerTimeoutSecs) ||
               (m_lastVoiceServerHeartbeatAckUtc.isValid() && m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) < ServerTimeoutSecs);
    }
} // ns
