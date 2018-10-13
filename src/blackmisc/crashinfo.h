/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CRASHINFO_H
#define BLACKMISC_CRASHINFO_H

#include "blackmisc/valueobject.h"
#include <QString>

namespace BlackMisc
{
    /*!
     * Crash info. Details about crash context.
     */
    class BLACKMISC_EXPORT CCrashInfo : public CValueObject<CCrashInfo>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexUserName = CPropertyIndex::GlobalIndexCCrashInfo,
            IndexInfo
        };

        //! Default constructor.
        CCrashInfo();

        //! Get user name
        const QString &getUserName() const { return m_userName; }

        //! User name
        void setUserName(const QString &userName) { m_userName = userName; }

        //! Get the info
        const QString &getInfo() const { return m_info; }

        //! Set info
        void setInfo(const QString &info) { m_info = info; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(const CPropertyIndex &index, const CCrashInfo &compareValue) const;

    private:
        QString m_userName;
        QString m_info;

        BLACK_METACLASS(
            CCrashInfo,
            BLACK_METAMEMBER(userName),
            BLACK_METAMEMBER(info)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CCrashInfo)

#endif // guard
