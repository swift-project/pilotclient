/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFTMODEL_H
#define BLACKMISC_AIRCRAFTMODEL_H

#include "nwuser.h"
#include "propertyindex.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Another pilot's aircraft model
         */
        class CAircraftModel : public CValueObjectStdTuple<CAircraftModel>
        {
        public:
            //! Indexes
            enum ColumnIndex
            {
                IndexModelString = BlackMisc::CPropertyIndex::GlobalIndexCAircraftModel,
                IndexIsQueriedModelString
            };

            //! Default constructor.
            CAircraftModel() : m_queriedModelStringFlag(false) {}

            //! Constructor.
            CAircraftModel(const QString &model, bool isQueriedString) : m_modelString(model), m_queriedModelStringFlag(isQueriedString) {}

            //! \copydoc CValueObject::propertyByIndex(int)
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(const QVariant, int)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Queried model string
            const QString &getModelString() const { return this->m_modelString; }

            //! Queried model string
            void setModelString(const QString &modelString) { this->m_modelString = modelString; }

            //! Set queried model string
            void setQueriedModelString(const QString &model) { this->m_modelString = model; }

            //! Queried model string?
            bool hasQueriedModelString() const { return this->m_queriedModelStringFlag && !this->m_modelString.isEmpty(); }

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftModel)
            QString m_modelString;
            bool m_queriedModelStringFlag; //!< model string is queried from network?
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CAircraftModel, (o.m_modelString, o.m_queriedModelStringFlag))
Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftModel)

#endif // guard
