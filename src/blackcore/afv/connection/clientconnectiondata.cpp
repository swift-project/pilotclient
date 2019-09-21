/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "clientconnectiondata.h"
#include <QDebug>

using namespace BlackCore::Afv::Crypto;

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            qint64 CClientConnectionData::secondsSinceAuthentication() const
            {
                return m_authenticatedDateTimeUtc.secsTo(QDateTime::currentDateTimeUtc());
            }

            bool CClientConnectionData::isVoiceServerAlive() const
            {
                return m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) > serverTimeout;
            }

            void CClientConnectionData::createCryptoChannels()
            {
                if (! m_tokens.isValid)
                {
                    qWarning() << "Tokens not set";
                }
                voiceCryptoChannel.reset(new CCryptoDtoChannel(m_tokens.VoiceServer.channelConfig));
                // dataCryptoChannel.reset(new CryptoDtoChannel(m_tokens.DataServer.channelConfig));
            }

            bool CClientConnectionData::voiceServerAlive() const
            {
                return timeSinceAuthentication() < serverTimeout ||
                       m_lastVoiceServerHeartbeatAckUtc.secsTo(QDateTime::currentDateTimeUtc()) < serverTimeout;
            }
        } // ns
    } // ns
} // ns
