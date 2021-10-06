/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_ECOSYSTEMLIST_H
#define BLACKMISC_NETWORK_ECOSYSTEMLIST_H

#include "blackmisc/network/ecosystem.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QStringList>
#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CEcosystem, CEcosystemList)

namespace BlackMisc::Network
{
    //! Value object encapsulating a list of voice rooms.
    class BLACKMISC_EXPORT CEcosystemList :
        public CSequence<CEcosystem>,
        public BlackMisc::Mixin::MetaType<CEcosystemList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CEcosystemList)
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
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CEcosystemList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CEcosystem>)

#endif //guard
