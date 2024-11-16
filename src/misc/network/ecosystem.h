// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_ECOSYSTEM_H
#define SWIFT_MISC_NETWORK_ECOSYSTEM_H

#include <QMetaType>
#include <QString>

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CEcosystem)

namespace swift::misc::network
{
    //! Ecosystem of server belonging together.
    class SWIFT_MISC_EXPORT CEcosystem : public CValueObject<CEcosystem>
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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CEcosystem &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
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

        SWIFT_METACLASS(
            CEcosystem,
            SWIFT_METAMEMBER(system));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CEcosystem)
Q_DECLARE_METATYPE(swift::misc::network::CEcosystem::System)

#endif // guard
