/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFTENGINELIST_H
#define BLACKMISC_AIRCRAFTENGINELIST_H

#include "aircraftengine.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft engines.
        class CAircraftEngineList : public CSequence<CAircraftEngine>
        {
        public:
            //! Default constructor.
            CAircraftEngineList() = default;

            //! Construct from a base class object.
            CAircraftEngineList(const CSequence<CAircraftEngine> &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            virtual QJsonObject toJson() const override
            {
                QJsonObject map;

                for (const auto &e : *this)
                {
                    QJsonObject value = e.toJson();
                    map.insert(QString::number(e.getNumber()), value);
                }
                return map;
            }

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override
            {
                clear();
                for (const auto &e : json.keys())
                {

                    CAircraftEngine engine;
                    int number = e.toInt();
                    engine.convertFromJson(json.value(e).toObject());
                    engine.setNumber(number);
                    push_back(engine);
                }
            }

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftEngineList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftEngine>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftEngine>)

#endif //guard
