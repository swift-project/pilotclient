/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTENGINELIST_H
#define BLACKMISC_AVIATION_AIRCRAFTENGINELIST_H

#include "aircraftengine.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <initializer_list>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft engines.
        class BLACKMISC_EXPORT CAircraftEngineList : public CSequence<CAircraftEngine>
        {
        public:
            //! Default constructor.
            CAircraftEngineList() = default;

            //! Construct by bool values for engines 1,2 ...
            CAircraftEngineList(std::initializer_list<bool> enginesOnOff);

            //! Construct from a base class object.
            CAircraftEngineList(const CSequence<CAircraftEngine> &other);

            //! Get engine 1..n
            CAircraftEngine getEngine(int engineNumber) const;

            //! Engine number 1..x on?
            bool isEngineOn(int engineNumber) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::toJson
            QJsonObject toJson() const;

            //! \copydoc CValueObject::convertFromJson
            void convertFromJson(const QJsonObject &json);

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftEngineList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftEngine>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftEngine>)

#endif //guard
