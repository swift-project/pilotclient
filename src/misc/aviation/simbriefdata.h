// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_SIMBRIEFDATA_H
#define SWIFT_MISC_AVIATION_SIMBRIEFDATA_H

#include <QString>
#include <QtGlobal>

#include "misc/datacache.h"
#include "misc/metaclass.h"
#include "misc/network/url.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CSimBriefData)

namespace swift::misc::aviation
{
    //! Value object for a flight plan
    class SWIFT_MISC_EXPORT CSimBriefData : public CValueObject<CSimBriefData>
    {
    public:
        //! The log. catgeories
        static const QStringList &getLogCategories();

        //! Default constructor
        CSimBriefData();

        //! Constructor
        CSimBriefData(const QString &url, const QString &username);

        //! Properties by index
        enum ColumnIndex
        {
            IndexUsername = CPropertyIndexRef::GlobalIndexCSimBriefData,
            IndexUrl
        };

        //! Get username
        const QString &getUsername() const { return m_username; }

        //! Set username
        void setUsername(const QString &un) { m_username = un; }

        //! Get URL
        const QString &getUrl() const { return m_url; }

        //! Get URL plus username
        network::CUrl getUrlAndUsername() const;

        //! Set URL
        void setUrl(const QString &url) { m_url = url; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_url;
        QString m_username;

        SWIFT_METACLASS(
            CSimBriefData,
            SWIFT_METAMEMBER(url),
            SWIFT_METAMEMBER(username));
    };

    namespace Data
    {
        //! Trait for global setup data
        struct TSimBriefData : public swift::misc::TDataTrait<CSimBriefData>
        {
            //! Key in data cache
            static const char *key() { return "simbriefdata"; }

            //! First load is synchronous
            static constexpr bool isPinned() { return true; }
        };
    } // namespace Data
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CSimBriefData)

#endif // guard
