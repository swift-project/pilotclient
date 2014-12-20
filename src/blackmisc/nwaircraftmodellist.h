/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_AIRCRAFTMODELLIST_H
#define BLACKMISC_AIRCRAFTMODELLIST_H

#include "nwaircraftmodel.h"
#include "collection.h"
#include "sequence.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating a list of aircraft models
         */
        class CAircraftModelList : public CSequence<CAircraftModel>
        {
        public:
            //! Empty constructor.
            CAircraftModelList();

            //! Construct from a base class object.
            CAircraftModelList(const CSequence<CAircraftModel> &other);

            //! QVariant, required for DBus QVariant lists
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Find by model string
            CAircraftModelList findByModelString(const QString modelString, Qt::CaseSensitivity sensitivity) const;

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftModelList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CAircraftModel>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CAircraftModel>)

#endif //guard
