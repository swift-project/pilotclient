/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
        //! \brief Aircraft mappings
        class CAircraftMappingList : public BlackMisc::CSequence<CAircraftMapping>
        {

        public:
            //! \brief Default constructor
            CAircraftMappingList() {}

            //! \brief Virtual destructor
            virtual ~CAircraftMappingList() {}

            //! \brief Unknown mapping
            static const CAircraftMapping &UnknownMapping()
            {
                static CAircraftMapping mapping;
                return mapping;
            }

            //! \brief Register metadata
            static void registerMetadata();
        };
    }
}

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftMappingList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::FsCommon::CAircraftMapping>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::FsCommon::CAircraftMapping>)

#endif // guard
