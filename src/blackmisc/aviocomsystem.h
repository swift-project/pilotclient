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
            BLACK_ENABLE_TUPLE_CONVERSION(CComSystem)
            ChannelSpacing m_channelSpacing;

            /*!
             * \brief Give me channel spacing in KHz
             * \remarks Just a helper method, that is why no CFrequency is returned
             */
            static double channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing);

        protected:
            //! \copydoc CAvionicsBase::validValues
            virtual bool validValues() const override;

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

        public:
            //! \brief Default constructor
            CComSystem() : CModulator(), m_channelSpacing(ChannelSpacing25KHz) {}

            //! \brief Constructor
            CComSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet(), int digits = 3):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency, digits), m_channelSpacing(ChannelSpacing25KHz)
            { }

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \brief Set active frequency
            //! \remarks will be rounded to channel spacing
            void setFrequencyActiveMHz(double frequencyMHz);

            //! \brief Set standby frequency
            //! \remarks will be rounded to channel spacing
            void setFrequencyStandbyMHz(double frequencyMHz);

            //! \brief Is active frequency within 8.3383kHz channel?
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return CComSystem::isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing8_33KHz);
            }

            //! \brief Is active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return CComSystem::isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing25KHz);
            }

            //! \brief Set UNICOM frequency as active
            void setActiveUnicom()
            {
                this->toggleActiveStandby();
                this->setFrequencyActive(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
            }

            //! \brief Set International Air Distress 121.5MHz
            void setActiveInternationalAirDistress()
            {
                this->toggleActiveStandby();
                this->setFrequencyActive(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress());
            }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! \brief Members
            static const QStringList &jsonMembers();

            //! \brief operator ==
            bool operator ==(const CComSystem &other) const;

            //! \brief operator !=
            bool operator !=(const CComSystem &other) const;

            //! \brief COM1 unit
            static CComSystem getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom1(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! \brief COM1 unit
            static CComSystem getCom1System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! \brief COM2 unit
            static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom2(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! \brief COM2 unit
            static CComSystem getCom2System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! \brief COM3 unit
            static CComSystem getCom3System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom3(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! \brief COM3 unit
            static CComSystem getCom3System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom3(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! \brief Valid civil aviation frequency?
            static bool isValidCivilAviationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 118.0 && fr <= 136.975;
            }

            //! \brief Valid military aviation frequency?
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

            //! \brief Is compareFrequency within channel spacing of setFrequency
            static bool isWithinChannelSpacing(const BlackMisc::PhysicalQuantities::CFrequency &setFrequency, const BlackMisc::PhysicalQuantities::CFrequency &compareFrequency, ChannelSpacing channelSpacing);

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CComSystem, (o.m_channelSpacing))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem)

#endif // include guard
