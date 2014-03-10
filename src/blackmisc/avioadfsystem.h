/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOADFSYSTEM_H
#define BLACKMISC_AVIOADFSYSTEM_H

#include "blackmisc/aviomodulator.h"
#include <stdexcept>

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * \brief ADF system ("for NDBs")
         */
        class CAdfSystem : public CModulator<CAdfSystem>
        {
        private:
            /*!
             * \brief Valid civil aviation frequency?
             * \param f
             * \return
             */
            bool isValidFrequency(PhysicalQuantities::CFrequency f) const
            {
                double fr = f.valueRounded(PhysicalQuantities::CFrequencyUnit::kHz(), this->m_digits);
                return fr >= 190.0 && fr <= 1750.0;
            }

            /*!
             * \brief Constructor
             * \param validate
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \param digits
             *
             */
            CAdfSystem(bool validate, const QString &name, const PhysicalQuantities::CFrequency &activeFrequency, const PhysicalQuantities::CFrequency &standbyFrequency, int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency, digits)
            {
                this->validate(validate);
            }

        protected:
            /*!
             * \copydoc CAvionicsBase::validValues
             */
            virtual bool validValues() const override
            {
                if (this->isDefaultValue()) return true; // special case
                return
                    this->isValidFrequency(this->getFrequencyActive()) &&
                    this->isValidFrequency(this->getFrequencyStandby());
            }

            /*!
             * \brief Validate values by assert and exception
             * \param strict
             * \throws std::range_error
             * \remarks Cannot be virtualsince already used in constructor
             * \return
             */
            bool validate(bool strict = true) const
            {
                if (this->isDefaultValue()) return true;
                bool valid = this->validValues();
                if (!strict) return valid;
                Q_ASSERT_X(valid, "CAdfSystem::validate", "illegal values");
                if (!valid) throw std::range_error("Illegal values in CAdfSystem::validate");
                return true;
            }

        public:
            /*!
             * Default constructor
             */
            CAdfSystem() : CModulator() {}

            /*!
             * \brief Copy constructor
             * \param other
             */
            CAdfSystem(const CAdfSystem &other) : CModulator(other) {}

            /*!
             * \brief Constructor
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \param digits
             */
            CAdfSystem(const QString &name, const PhysicalQuantities::CFrequency &activeFrequency, const PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet(), int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency, digits)
            {
                this->validate(true);
            }

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Set active frequency
             * \param frequencyKHz
             */
            void setFrequencyActiveKHz(double frequencyKHz)
            {
                this->CModulator::setFrequencyActiveKHz(frequencyKHz);
                this->validate(true);
            }

            /*!
             * \brief Set standby frequency
             * \param frequencyKHz
             */
            void setFrequencyStandbyKHz(double frequencyKHz)
            {
                this->CModulator::setFrequencyStandbyKHz(frequencyKHz);
                this->validate(true);
            }

            /*!
             * \brief operator ==
             * \param otherSystem
             * \return
             */
            bool operator ==(const CAdfSystem &otherSystem) const
            {
                return this->CModulator::operator ==(otherSystem);
            }

            /*!
             * \brief operator ==
             * \param otherSystem
             * \return
             */
            bool operator !=(const CAdfSystem &otherSystem) const
            {
                return this->CModulator::operator !=(otherSystem);
            }

            /*!
             * Try to get a ADF unit with given name and frequency. Returns true in case an object
             * has been sucessfully created, otherwise returns a default object and false.
             * \param[out] o_adfSystem
             * \param name
             * \param activeFrequencyKHz
             * \param standbyFrequencyKHz
             * \return
             */
            static bool tryGetAdfSystem(CAdfSystem &o_adfSystem, const QString &name, double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                o_adfSystem = CAdfSystem(false, name, PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
                bool s;
                if (!(s = o_adfSystem.validate(false))) o_adfSystem = CAdfSystem(); // reset to default
                return s;
            }

            /*!
             * Try to get a ADF unit with given name and frequency. Returns true in case an object
             * has been sucessfully created, otherwise returns a default object.
             * \param[out] o_adfSystem
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetAdfSystem(CAdfSystem &o_adfSystem, const QString &name, PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                o_adfSystem = CAdfSystem(false, name, activeFrequency, standbyFrequency);
                bool s;
                if (!(s = o_adfSystem.validate(false))) o_adfSystem = CAdfSystem(); // reset to default
                return s;
            }

            /*!
             * \brief ADF1 unit
             * \param activeFrequencyKHz
             * \param standbyFrequencyKHz
             * \return
             */
            static CAdfSystem GetAdf1System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem(CModulator::NameCom1(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            /*!
             * \brief ADF1 unit
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static CAdfSystem GetAdf1System(PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            /*!
             * \brief Try to get ADF unit
             * \param[out] o_adfSystem
             * \param activeFrequencyKHz
             * \param standbyFrequencyKHz
             * \return
             */
            static bool tryGetAdf1Unit(CAdfSystem &o_adfSystem, double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem::tryGetAdfSystem(o_adfSystem, CModulator::NameCom1(), activeFrequencyKHz, standbyFrequencyKHz);
            }

            /*!
             * \brief Try to get ADF unit
             * \param[out] o_adfSystem
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetAdf1Unit(CAdfSystem &o_adfSystem, PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem::tryGetAdfSystem(o_adfSystem, CModulator::NameCom1(), activeFrequency, standbyFrequency);
            }

            /*!
             * \brief ADF2 unit
             * \param activeFrequencyKHz
             * \param standbyFrequencyKHz
             * \return
             */
            static CAdfSystem GetAdf2System(double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem(CModulator::NameCom2(), PhysicalQuantities::CFrequency(activeFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()), PhysicalQuantities::CFrequency(standbyFrequencyKHz < 0 ? activeFrequencyKHz : standbyFrequencyKHz, PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            /*!
             * \brief ADF2 unit
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static CAdfSystem GetAdf2System(PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            /*!
             * \brief Try to get ADF unit
             * \param[out] o_adfSystem
             * \param activeFrequencyKHz
             * \param standbyFrequencyKHz
             * \return
             */
            static bool tryGetAdf2System(CAdfSystem &o_adfSystem, double activeFrequencyKHz, double standbyFrequencyKHz = -1)
            {
                return CAdfSystem::tryGetAdfSystem(o_adfSystem, CModulator::NameCom2(), activeFrequencyKHz, standbyFrequencyKHz);
            }

            /*!
             * \brief Try to get ADF unit
             * \param[out] o_adfSystem
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetAdf2System(CAdfSystem &o_adfSystem, PhysicalQuantities::CFrequency activeFrequency, PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CAdfSystem::tryGetAdfSystem(o_adfSystem, CModulator::NameCom2(), activeFrequency, standbyFrequency);
            }
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAdfSystem)

#endif // guard
