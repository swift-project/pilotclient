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
        public:
            /*!
             * Channel spacing frequency
             */
            enum ChannelSpacing
            {
                ChannelSpacing50KHz,
                ChannelSpacing25KHz,
                ChannelSpacing8_33KHz
            };

        private:
            ChannelSpacing m_channelSpacing;

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
                CModulator(name, activeFrequency, standbyFrequency, digits), m_channelSpacing(ChannelSpacing25KHz)
            {
                this->validate(validate);
            }

            /*!
             * \brief Give me channel spacing in KHz
             * \remarks Just a helper method, that is why no CFrequency is returned
             */
            static double channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing);

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

            /*!
             * \copydoc CValueObject::marshallFromDbus()
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \copydoc CValueObject::unmarshallFromDbus()
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

        public:
            /*!
             * Default constructor
             */
            CComSystem() : CModulator(), m_channelSpacing(ChannelSpacing25KHz) {}

            /*!
             * \brief Constructor
             */
            CComSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet(), int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency, digits), m_channelSpacing(ChannelSpacing25KHz)
            {
                this->validate(true);
            }

            /*!
             * \brief Copy constructor
             */
            CComSystem(const CComSystem &other) : CModulator(other), m_channelSpacing(other.m_channelSpacing) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \copydoc CValueObject::getValueHash
             * \return
             */
            virtual uint getValueHash() const
            {
                return CModulator::getValueHash();
            }

            /*!
             * \brief Set active frequency
             * \remarks will be rounded to channel spacing
             * \see ChannelSpacing
             */
            void setFrequencyActiveMHz(double frequencyMHz);

            /*!
             * \brief Set standby frequency
             * \remarks will be rounded to channel spacing
             */
            void setFrequencyStandbyMHz(double frequencyMHz);

            /*!
             * \brief Is active frequency within 8.3383kHz channel?
             */
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency)
            {
                return CComSystem::isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing8_33KHz);
            }

            /*!
             * \brief Is active frequency within 25kHz channel?
             */
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return CComSystem::isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing25KHz);
            }

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
             */
            bool operator ==(const CComSystem &other) const
            {
                return this->CModulator::operator ==(other);
            }

            /*!
             * \brief operator !=
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
             */
            static bool isValidCivilAviationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 118.0 && fr <= 136.975;
            }

            /*!
             * \brief Valid military aviation frequency?
             */
            static bool isValidMilitaryFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 220.0 && fr <= 399.95;
            }

            /*!
             * \brief Round to channel spacing, set MHz as unit
             * \see ChannelSpacing
             */
            static void roundToChannelSpacing(BlackMisc::PhysicalQuantities::CFrequency &frequency, ChannelSpacing channelSpacing);

            /*!
             * \brief Is compareFrequency within channel spacing of setFrequency
             */
            static bool isWithinChannelSpacing(const BlackMisc::PhysicalQuantities::CFrequency &setFrequency, const BlackMisc::PhysicalQuantities::CFrequency &compareFrequency, ChannelSpacing channelSpacing);

        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem)

#endif // include guard
