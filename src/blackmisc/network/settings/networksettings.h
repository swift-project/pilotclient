/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_SETTINGS_NETWORKSETTINGS_H
#define BLACKMISC_NETWORK_SETTINGS_NETWORKSETTINGS_H

#include "blackmisc/propertyindexref.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

namespace BlackMisc::Network::Settings
{
    //! Network settings
    //! \remark CURRENTLY a STUB not used
    class BLACKMISC_EXPORT CNetworkSettings : public CValueObject<CNetworkSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDynamicOffsetTime = CPropertyIndexRef::GlobalIndexCNetworkSettings,
        };

        //! Constructor
        CNetworkSettings() {}

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

    private:
        bool m_dummy = false;

        BLACK_METACLASS(
            CNetworkSettings,
            BLACK_METAMEMBER(dummy)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Network::Settings::CNetworkSettings)

#endif // guard
