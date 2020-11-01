/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_SIMBRIEFDATA_H
#define BLACKMISC_AVIATION_SIMBRIEFDATA_H

#include "blackmisc/network/url.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/datacache.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a flight plan
        class BLACKMISC_EXPORT CSimBriefData : public CValueObject<CSimBriefData>
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
                IndexUsername = CPropertyIndex::GlobalIndexCSimBriefData,
                IndexUrl
            };

            //! Get username
            const QString &getUsername() const { return m_username; }

            //! Set username
            void setUsername(const QString &un) { m_username = un; }

            //! Get URL
            const QString &getUrl() const { return m_url; }

            //! Get URL plus username
            Network::CUrl getUrlAndUsername() const;

            //! Set URL
            void setUrl(const QString &url) { m_url = url; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

        private:
            QString m_url;
            QString m_username;

            BLACK_METACLASS(
                CSimBriefData,
                BLACK_METAMEMBER(url),
                BLACK_METAMEMBER(username)
            );
        };

        namespace Data
        {
            //! Trait for global setup data
            struct TSimBriefData : public BlackMisc::TDataTrait<CSimBriefData>
            {
                //! Key in data cache
                static const char *key() { return "simbriefdata"; }

                //! First load is synchronous
                static constexpr bool isPinned() { return true; }
            };
        }
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CSimBriefData)

#endif // guard
