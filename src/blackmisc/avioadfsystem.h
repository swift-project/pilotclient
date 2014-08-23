/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIOADFSYSTEM_H
#define BLACKMISC_AVIOADFSYSTEM_H

#include "blackmisc/aviomodulator.h"
#include <stdexcept>

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * ADF system ("for NDBs")
         */
        class CAdfSystem : public CModulator<CAdfSystem>
        {

        public:
            //! Default constructor
            CAdfSystem() : CModulator() {}

            //! Constructor
            CAdfSystem(const QString &name, const PhysicalQuantities::CFrequency &activeFrequency, const PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet(), int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency, digits)
            { }

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Valid aviation frequency?
            bool isValidFrequency(PhysicalQuantities::CFrequency f) const
            {
                double fr = f.valueRounded(PhysicalQuantities::CFrequencyUnit::kHz(), this->m_digits);
                return fr >= 190.0 && fr <= 1750.0;
            }

            //! Equal operator ==
            bool operator ==(const CAdfSystem &otherSystem) const
            {
                return this->CModulator::operator ==(otherSystem);
            }

            //! Equal operator !=
            bool operator !=(const CAdfSystem &otherSystem) const
            {
                return this->CModulator::operator !=(otherSystem);
            }


            //! ADF1 unit
            static CAdfSystem GetAdf1System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem(CModulator::NameCom1(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! ADF1 unit
            static CAdfSystem GetAdf1System(PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! ADF2 unit
            static CAdfSystem GetAdf2System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem(CModulator::NameCom2(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! ADF2 unit
            static CAdfSystem GetAdf2System(PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

        protected:
            //! \copydoc CAvionicsBase::validValues
            virtual bool validValues() const override
            {
                if (this->isDefaultValue()) return true; // special case
                return
                    this->isValidFrequency(this->getFrequencyActive()) &&
                    this->isValidFrequency(this->getFrequencyStandby());
            }

        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAdfSystem)

#endif // guard
