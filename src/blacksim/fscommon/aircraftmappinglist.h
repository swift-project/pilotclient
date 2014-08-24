/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTMAPPINGS_H
#define BLACKSIM_FSCOMMON_AIRCRAFTMAPPINGS_H

#include "aircraftmapping.h"
#include "aircraftcfgentrieslist.h"
#include "blackmisc/sequence.h"
#include <QTemporaryFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>

namespace BlackSim
{
    namespace FsCommon
    {
        //! Aircraft mappings
        class CAircraftMappingList : public BlackMisc::CSequence<CAircraftMapping>
        {

        public:
            //! Default constructor
            CAircraftMappingList() {}

            //! Virtual destructor
            virtual ~CAircraftMappingList() {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Unknown mapping
            static const CAircraftMapping &UnknownMapping()
            {
                static CAircraftMapping mapping;
                return mapping;
            }

            //! Register metadata
            static void registerMetadata();
        };
    }
}

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftMappingList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::FsCommon::CAircraftMapping>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::FsCommon::CAircraftMapping>)

#endif // guard
