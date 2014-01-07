/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_AVIOCOMSYSTEM_H
#define BLACKMISC_AVIOCOMSYSTEM_H
#include "blackmisc/aviomodulator.h"
#include <stdexcept>

namespace BlackMisc
{
    namespace Aviation
    {

        /*!
         * \brief COM system (aka "radio")
         */
        class CComSystem : public CModulator<CComSystem>
        {
        private:
            /*!
             * \brief Constructor
             * \param validate
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \param digits
             *
             */
            CComSystem(bool validate, const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency, int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency, digits)
            {
                this->validate(validate);
            }

        protected:
            /*!
             * \brief Are the set values valid / in range?
             * \return
             */
            bool validValues() const;

            /*!
             * \brief Validate values by assert and exception
             * \param strict
             * \throws std::range_error
             * \remarks Cannot be virtual because used in constructor
             * \return
             */
            bool validate(bool strict = true) const;

        public:
            /*!
             * Default constructor
             */
            CComSystem() : CModulator() {}

            /*!
             * \brief Copy constructor
             * \param other
             */
            CComSystem(const CComSystem &other) : CModulator(other) {}

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Value hash
             * \return
             */
            virtual uint getValueHash() const
            {
                return CModulator::getValueHash();
            }

            /*!
             * \brief Constructor
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \param digits
             */
            CComSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet(), int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency, digits)
            {
                this->validate(true);
            }

            /*!
             * \brief Set active frequency
             * \param frequencyMHz
             */
            void setFrequencyActiveMHz(double frequencyMHz);

            /*!
             * \brief Set standby frequency
             * \param frequencyMHz
             */
            void setFrequencyStandbyMHz(double frequencyMHz);

            /*!
             * \brief Set UNICOM frequency as active
             */
            void setActiveUnicom()
            {
                this->toggleActiveStandby();
                this->setFrequencyActive(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
            }

            /*!
             * \brief Set International Air Distress 121.5MHz
             */
            void setActiveInternationalAirDistress()
            {
                this->toggleActiveStandby();
                this->setFrequencyActive(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress());
            }

            /*!
             * \brief operator ==
             * \param other
             * \return
             */
            bool operator ==(const CComSystem &other) const
            {
                return this->CModulator::operator ==(other);
            }

            /*!
             * \brief operator !=
             * \param other
             * \return
             */
            bool operator !=(const CComSystem &other) const
            {
                return this->CModulator::operator !=(other);
            }

            /*!
             * Try to get a COM unit with given name and frequency. Returns true in case an object
             * has been sucessfully created, otherwise returns a default object.
             * \param[out] o_comSystem
             * \param name
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static bool tryGetComSystem(CComSystem &o_comSystem, const QString &name, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                o_comSystem = CComSystem(false, name, BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
                bool s;
                if (!(s = o_comSystem.validate(false))) o_comSystem = CComSystem(); // reset to default
                return s;
            }

            /*!
             * Try to get a COM unit with given name and frequency. Returns true in case an object
             * has been sucessfully created, otherwise returns a default object.
             * \param[out] o_comSystem
             * \param name
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetComSystem(CComSystem &o_comSystem, const QString &name, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                o_comSystem = CComSystem(false, name, activeFrequency, standbyFrequency);
                bool s;
                if (!(s = o_comSystem.validate(false))) o_comSystem = CComSystem(); // reset to default
                return s;
            }

            /*!
             * \brief COM1 unit
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static CComSystem getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom1(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            /*!
             * \brief COM1 unit
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static CComSystem getCom1System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            /*!
             * \brief Try to get COM unit
             * \param[out] o_comSystem
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static bool tryGetCom1Unit(CComSystem &o_comSystem, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem::tryGetComSystem(o_comSystem, CModulator::NameCom1(), activeFrequencyMHz, standbyFrequencyMHz);
            }

            /*!
             * \brief Try to get COM unit
             * \param[out] o_comSystem
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetCom1Unit(CComSystem &o_comSystem, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem::tryGetComSystem(o_comSystem, CModulator::NameCom1(), activeFrequency, standbyFrequency);
            }

            /*!
             * \brief COM2 unit
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom2(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            /*!
             * \brief COM2 unit
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static CComSystem getCom2System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            /*!
             * \brief Try to get COM unit
             * \param[out] o_comSystem
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static bool tryGetCom2System(CComSystem &o_comSystem, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem::tryGetComSystem(o_comSystem, CModulator::NameCom2(), activeFrequencyMHz, standbyFrequencyMHz);
            }

            /*!
             * \brief Try to get COM unit
             * \param[out] o_comSystem
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetCom2System(CComSystem &o_comSystem, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem::tryGetComSystem(o_comSystem, CModulator::NameCom2(), activeFrequency, standbyFrequency);
            }

            /*!
             * \brief COM3 unit
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static CComSystem getCom3System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom3(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            /*!
             * \brief COM3 unit
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static CComSystem getCom3System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom3(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            /*!
             * \brief Try to get COM unit
             * \param[out] o_comSystem
             * \param activeFrequencyMHz
             * \param standbyFrequencyMHz
             * \return
             */
            static bool tryGetCom3System(CComSystem &o_comSystem, double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem::tryGetComSystem(o_comSystem, CModulator::NameCom3(), activeFrequencyMHz, standbyFrequencyMHz);
            }

            /*!
             * \brief Try to get COM unit
             * \param[out] o_comSystem
             * \param activeFrequency
             * \param standbyFrequency
             * \return
             */
            static bool tryGetCom3System(CComSystem &o_comSystem, BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem::tryGetComSystem(o_comSystem, CModulator::NameCom3(), activeFrequency, standbyFrequency);
            }

            /*!
             * \brief Valid civil aviation frequency?
             * \param f
             * \return
             */
            static bool isValidCivilAviationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 118.0 && fr <= 136.975;
            }

            /*!
             * \brief Valid military aviation frequency?
             * \param f
             * \return
             */
            static bool isValidMilitaryFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 220.0 && fr <= 399.95;
            }

            /*!
             * \brief Round to 25KHz
             * \param f
             * \return
             */
            static void roundTo25KHz(BlackMisc::PhysicalQuantities::CFrequency &frequency);

        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem)

#endif // include guard
