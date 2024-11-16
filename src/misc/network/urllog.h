// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_URLLOG_H
#define SWIFT_MISC_NETWORK_URLLOG_H

#include <QJsonObject>
#include <QString>

#include "misc/network/url.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CUrlLog)

namespace swift::misc::network
{
    //! Information about accessing one URL over the network
    class SWIFT_MISC_EXPORT CUrlLog :
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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! Property name used for request
        static const char *propertyNameId();

    private:
        int m_id = -1;
        CUrl m_url;
        bool m_success = false;
        qint64 m_responseTimeMSecsSinceEpoch = -1;
        qint64 m_responseTimeMs = -1;

        //! Get a unique id
        //! \threadsafe
        static int uniqueId();

        SWIFT_METACLASS(
            CUrlLog,
            SWIFT_METAMEMBER(id),
            SWIFT_METAMEMBER(url),
            SWIFT_METAMEMBER(success),
            SWIFT_METAMEMBER(responseTimeMSecsSinceEpoch),
            SWIFT_METAMEMBER(responseTimeMs));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CUrlLog)

#endif // guard
