/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTENGINELIST_H
#define BLACKMISC_AVIATION_AIRCRAFTENGINELIST_H

#include "blackmisc/aviation/aircraftengine.h"
#include "blackmisc/aviation/aircraftengine.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/json.h"
#include "blackmisc/sequence.h"

#include <QJsonObject>
#include <QMetaType>
#include <initializer_list>
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft engines.
        class BLACKMISC_EXPORT CAircraftEngineList :
            public CSequence<CAircraftEngine>,
            public Mixin::MetaType<CAircraftEngineList>,
            public Mixin::JsonOperators<CAircraftEngineList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftEngineList)
            using CSequence::CSequence;

            //! Default constructor.
            CAircraftEngineList() = default;

            //! Construct by bool values for engines 1,2 ...
            CAircraftEngineList(std::initializer_list<bool> enginesOnOff);

            //! Construct from a base class object.
            CAircraftEngineList(const CSequence<CAircraftEngine> &other);

            //! Get engine 1..n
            //! \remark 1 based, not 0 based
            CAircraftEngine getEngine(int engineNumber) const;

            //! Engine number 1..x on?
            //! \remark 1 based, not 0 based
            bool isEngineOn(int engineNumber) const;

            //! Set engine on/off
            //! \remark 1 based, not 0 based
            void setEngineOn(int engineNumber, bool on);

            //! Copy one engine multiple times (number) and set the number of engines to number
            void setEngines(const CAircraftEngine &engine, int engineNumber);

            //! Init some engines
            void initEngines(int engineNumber, bool on);

            //! Is any engine on?
            bool isAnyEngineOn() const;

            //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJson
            QJsonObject toJson() const;

            //! \copydoc BlackMisc::Mixin::JsonByMetaClass::convertFromJson
            void convertFromJson(const QJsonObject &json);
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftEngineList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftEngine>)

#endif //guard
