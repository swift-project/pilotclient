/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_SETTINGS_NETWORKSETTINGS_H
#define BLACKMISC_NETWORK_SETTINGS_NETWORKSETTINGS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QJsonObject>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        namespace Settings
        {
            //! Network settings
            class BLACKMISC_EXPORT CNetworkSettings : public CValueObject<CNetworkSettings>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexDynamicOffsetTime = CPropertyIndex::GlobalIndexCNetworkSettings,
                };

                //! Constructor
                CNetworkSettings() {}

                //! Dynamic offset values?
                bool isUsingDynamicOffsetTimes() const { return m_dynamicOffsetTimes; }

                //! Dynamic offset values?
                void setDynamicOffsetTimes(bool dynamic) { m_dynamicOffsetTimes = dynamic; }

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                CVariant propertyByIndex(const CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            private:
                bool m_dynamicOffsetTimes = false;

                BLACK_METACLASS(
                    CNetworkSettings,
                    BLACK_METAMEMBER(dynamicOffsetTimes)
                );
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Network::Settings::CNetworkSettings)

#endif // guard
