/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_URLLOG_H
#define BLACKMISC_NETWORK_URLLOG_H

#include "url.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindexref.h"

#include <QJsonObject>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        //! Information about accessing one URL over the network
        class BLACKMISC_EXPORT CUrlLog :
            public CValueObject<CUrlLog>,
            public ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexId = CPropertyIndexRef::GlobalIndexCUrlLog,
                IndexSuccess,
                IndexUrl,
                IndexResponseTimestamp,
                IndexResponseTime
            };

            //! Constructor, setting created to now and getting a valid id
            CUrlLog(const CUrl &url = {});

            //! Unique id
            int getId() const { return this->m_id; }

            //! Get URL.
            const CUrl &getUrl() const { return m_url; }

            //! Response timestamp
            QDateTime getResponseTimestamp() const { return QDateTime::fromMSecsSinceEpoch(m_responseTimeMSecsSinceEpoch); }

            //! Response timestamp milliseconds since epoch
            qint64 getResponseTimeMSecsSinceEpoch() const { return m_responseTimeMSecsSinceEpoch; }

            //! Set response time and response timestamp
            void setResponseTimestampToNow();

            //! Response time
            qint64 getResponseTimeMs() const { return m_responseTimeMs; }

            //! Pending
            bool isPending() const;

            //! Success?
            bool isSuccess() const { return m_success; }

            //! Set success
            void setSuccess(bool s) { m_success = s; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! Property name used for request
            static const char *propertyNameId();

        private:
            int    m_id = -1;
            CUrl   m_url;
            bool   m_success = false;
            qint64 m_responseTimeMSecsSinceEpoch = -1;
            qint64 m_responseTimeMs = -1;

            //! Get a unique id
            //! \threadsafe
            static int uniqueId();

            BLACK_METACLASS(
                CUrlLog,
                BLACK_METAMEMBER(id),
                BLACK_METAMEMBER(url),
                BLACK_METAMEMBER(success),
                BLACK_METAMEMBER(responseTimeMSecsSinceEpoch),
                BLACK_METAMEMBER(responseTimeMs)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUrlLog)

#endif // guard
