/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODEL_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODEL_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraft.h"
#include "blackmisc/aviation/aircrafticao.h"
#include "blackmisc/network/user.h"
#include "blackmisc/propertyindex.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Aircraft model (other pilot, my models on disk)
        //! \remarks Simulator independent class, supposed to be common denominator
        class BLACKMISC_EXPORT CAircraftModel : public CValueObject<CAircraftModel>
        {
        public:
            //! Model type
            enum ModelType
            {
                TypeUnknown,
                TypeQueriedFromNetwork, //!< model was queried by network protocol
                TypeModelMatching,      //!< model is result of model matching
                TypeModelMatchingDefaultModel, //!< a default model assigned by model matching
                TypeModelMapping,       //!< used along with mapping definition
                TypeManuallySet,        //!< manually set, e.g. from GUI
                TypeOwnSimulatorModel   //!< represents own simulator model
            };

            //! Indexes
            enum ColumnIndex
            {
                IndexModelString = BlackMisc::CPropertyIndex::GlobalIndexCAircraftModel,
                IndexCallsign,
                IndexDescription,
                IndexIcao,
                IndexFileName,
                IndexModelType,
                IndexModelTypeAsString,
                IndexHasQueriedModelString
            };

            //! Default constructor.
            CAircraftModel() {}

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type) : m_modelString(model), m_modelType(type) {}

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const QString &description, const BlackMisc::Aviation::CAircraftIcao &icao) :
                m_icao(icao), m_modelString(model), m_description(description), m_modelType(type) {}

            //! Constructor
            CAircraftModel(const BlackMisc::Aviation::CAircraft &aircraft);

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Corresponding callsign if applicable
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_callsign; }

            //! Corresponding callsign if applicable
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { this->m_callsign = callsign; }

            //! Callsign empty
            bool isCallsignEmpty() const { return this->m_callsign.isEmpty(); }

            //! Queried model string
            const QString &getModelString() const { return this->m_modelString; }

            //! Model string
            void setModelString(const QString &modelString) { this->m_modelString = modelString; }

            //! Descriptive text
            const QString &getDescription() const { return this->m_description; }

            //! Descriptive text
            void setDescription(const QString &description) { this->m_description = description; }

            //! Set queried model string
            void setQueriedModelString(const QString &model) { this->m_modelString = model; this->m_modelType = TypeQueriedFromNetwork; }

            //! ICAO code
            BlackMisc::Aviation::CAircraftIcao getIcao() const { return this->m_icao; }

            //! Set ICAO info
            void setIcao(const BlackMisc::Aviation::CAircraftIcao &icao) { this->m_icao = icao; }

            //! \copydoc CAircraftIcao::hasAircraftAndAirlineDesignator
            bool hasAircraftAndAirlineDesignator() const { return this->m_icao.hasAircraftAndAirlineDesignator(); }

            //! \copydoc CAircraftIcao::hasAircraftDesignator
            bool hasAircraftDesignator() const { return this->m_icao.hasAircraftDesignator(); }

            //! Model type
            ModelType getModelType() const { return static_cast<ModelType>(m_modelType); }

            //! Model type
            QString getModelTypeAsString() const { return modelTypeToString(getModelType()); }

            //! Set type
            void setModelType(ModelType type) { this->m_modelType = static_cast<int>(type); }

            //! File name
            QString getFileName() const { return m_fileName; }

            //! File name
            void setFileName(const QString &fileName) { m_fileName = fileName; }

            //! Update missing parts from another model
            void updateMissingParts(const CAircraftModel &model);

            //! Queried model string?
            bool hasQueriedModelString() const;

            //! Model string which was manually set
            bool hasManuallySetString() const;

            //! Non empty model string
            bool hasModelString() const { return !m_modelString.isEmpty(); }

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Model type
            static QString modelTypeToString(ModelType type);

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftModel)
            BlackMisc::Aviation::CCallsign m_callsign; //!< aircraft's callsign
            BlackMisc::Aviation::CAircraftIcao m_icao; //!< ICAO data if available
            QString m_modelString;                     //!< Simulator model string
            QString m_description;                     //!< descriptive text
            QString m_fileName;                        //!< file name
            int m_modelType = static_cast<int>(TypeUnknown);  //!< model string is queried from network?
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(
    BlackMisc::Simulation::CAircraftModel, (
        attr(o.m_callsign),
        attr(o.m_icao),
        attr(o.m_modelString, flags<CaseInsensitiveComparison>()),
        attr(o.m_description, flags<DisabledForComparison>()),
        attr(o.m_fileName, flags <DisabledForComparison> ()),
        attr(o.m_modelType)
    ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel)

#endif // guard
