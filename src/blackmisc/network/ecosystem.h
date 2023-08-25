// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_ECOSYSTEM_H
#define BLACKMISC_NETWORK_ECOSYSTEM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CEcosystem)

namespace BlackMisc::Network
{
    //! Ecosystem of server belonging together.
    class BLACKMISC_EXPORT CEcosystem : public CValueObject<CEcosystem>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexSystem = CPropertyIndexRef::GlobalIndexCEcosystem,
            IndexSystemString
        };

        //! Known systems
        enum System
        {
            Unspecified, //!< unspecified
            NoSystem, //!< no relevant ecosystem
            VATSIM, //!< VATSIM
            SwiftTest, //!< swift test server
            Swift, //!< Future usage
            PrivateFSD //!< Private FSD environment
        };

        //! Default constructor
        CEcosystem() {}

        //! Constructor
        CEcosystem(System s) : m_system(static_cast<int>(s)) {}

        //! Get system
        System getSystem() const { return static_cast<System>(m_system); }

        //! Unknown system?
        bool isUnspecified() const { return this->getSystem() == Unspecified; }

        //! Is system?
        bool isSystem(System s) const { return this->getSystem() == s; }

        //! Set the system
        void setSystem(System system) { m_system = static_cast<int>(system); }

        //! Get the system string
        const QString &getSystemString() const;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CEcosystem &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Unspecified
        static const CEcosystem &unspecified();

        //! VATSIM eco system
        static const CEcosystem &vatsim();

        //! swift eco system
        static const CEcosystem &swift();

        //! swift test eco system
        static const CEcosystem &swiftTest();

        //! FSD private
        static const CEcosystem &privateFsd();

    private:
        int m_system = static_cast<int>(Unspecified);

        BLACK_METACLASS(
            CEcosystem,
            BLACK_METAMEMBER(system)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CEcosystem)
Q_DECLARE_METATYPE(BlackMisc::Network::CEcosystem::System)

#endif // guard
