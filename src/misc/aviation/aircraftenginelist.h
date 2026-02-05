// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTENGINELIST_H
#define SWIFT_MISC_AVIATION_AIRCRAFTENGINELIST_H

#include <initializer_list>
#include <tuple>

#include <QJsonObject>
#include <QMetaType>

#include "misc/aviation/aircraftengine.h"
#include "misc/collection.h"
#include "misc/json.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftEngine, CAircraftEngineList)

namespace swift::misc::aviation
{
    //! Value object encapsulating a list of aircraft engines.
    class SWIFT_MISC_EXPORT CAircraftEngineList :
        public CSequence<CAircraftEngine>,
        public mixin::MetaType<CAircraftEngineList>,
        public mixin::JsonOperators<CAircraftEngineList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftEngineList)
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
        void initEngines(int engineNumber, bool on, double enginePercentage);

        //! Is any engine on?
        bool isAnyEngineOn() const;

        void setEnginePower(int engineNumber, double percentage);

        double getEnginePower(int engineNumber) const;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! Get engine count
        int getEngineCount() const { return this->size(); }
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftEngineList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAircraftEngine>)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTENGINELIST_H
