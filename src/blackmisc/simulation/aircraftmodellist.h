/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFTMODELLIST_H
#define BLACKMISC_AIRCRAFTMODELLIST_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

namespace BlackMisc
{
    namespace Simulation
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

            //! Contains model string
            bool containsModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find by model string
            CAircraftModelList findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by model string
            CAircraftModel findFirstByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find models starting with
            CAircraftModelList findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Model strings
            QStringList getSortedModelStrings() const;

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModelList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CAircraftModel>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CAircraftModel>)

#endif //guard
