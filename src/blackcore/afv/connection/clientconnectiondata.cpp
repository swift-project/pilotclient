/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "clientconnectiondata.h"
#include "blackmisc/logmessage.h"
#include <QDebug>

using namespace BlackMisc;
using namespace BlackCore::Afv::Crypto;

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            const CLogCategoryList &CClientConnectionData::getLogCategories()
            {
                static const CLogCategoryList cats { CLogCategory::audio(), CLogCategory::vatsimSpecific() };
                return cats;
            }

            qint64 CClientConnectionData::secondsSinceAuthentication() const
            {
                return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc());
            }

            bool CClientConnectionData::isVoiceServerAlive() const
            {
                return m_lastVoiceServerHeartbeatAckUtc.isValid() &&
                       m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) > ServerTimeoutSecs;
            }

            bool CClientConnectionData::isDataServerAlive() const
            {
                return m_lastDataServerHeartbeatAckUtc.isValid() &&
                       m_lastDataServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) > ServerTimeoutSecs;
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
                m_lastDataServerHeartbeatAckUtc  = now;
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
    } // ns
} // ns
