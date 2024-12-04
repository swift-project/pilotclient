// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_ECOSYSTEMLIST_H
#define SWIFT_MISC_NETWORK_ECOSYSTEMLIST_H

#include <QMetaType>
#include <QStringList>

#include "misc/collection.h"
#include "misc/network/ecosystem.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CEcosystem, CEcosystemList)

namespace swift::misc::network
{
    //! Value object encapsulating a list of voice rooms.
    class SWIFT_MISC_EXPORT CEcosystemList :
        public CSequence<CEcosystem>,
        public swift::misc::mixin::MetaType<CEcosystemList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CEcosystemList)
        using CSequence::CSequence;

        //! Default constructor.
        CEcosystemList();

        //! Construct from a base class object.
        CEcosystemList(const CSequence &other);

        //! All system strings
        QStringList allSystemStrings() const;

        //! All systems
        static const CEcosystemList &allKnownSystems();
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CEcosystemList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CEcosystem>)

#endif // SWIFT_MISC_NETWORK_ECOSYSTEMLIST_H
