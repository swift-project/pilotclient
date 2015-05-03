/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_COMSYSTEM_H
#define BLACKMISC_AVIATION_COMSYSTEM_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/modulator.h"

namespace BlackMisc
{
    namespace Aviation
    {

        //! COM system (aka "radio")
        class BLACKMISC_EXPORT CComSystem :
            public CModulator<CComSystem>,
            public Mixin::MetaType<CComSystem>,
            public Mixin::JsonOperators<CComSystem>
        {
        public:
            //! Base type
            using base_type = CModulator<CComSystem>;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CComSystem)

            //! Channel spacing frequency
            enum ChannelSpacing
            {
                ChannelSpacing50KHz,
                ChannelSpacing25KHz,
                ChannelSpacing8_33KHz
            };

            //! COM unit
            enum ComUnit
            {
                Com1,
                Com2
            };

            //! Default constructor
            CComSystem() {}

            //! Constructor
            CComSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = CModulator::FrequencyNotSet()):
                CModulator(name, activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency)
            { }

            //! Set active frequency
            //! \remarks will be rounded to channel spacing
            virtual void setFrequencyActiveMHz(double frequencyMHz) override;

            //! Set standby frequency
            //! \remarks will be rounded to channel spacing
            virtual void setFrequencyStandbyMHz(double frequencyMHz) override;

            //! Set active frequency
            //! \remarks will be rounded to channel spacing
            virtual void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency) override;

            //! Set active frequency
            //! \remarks will be rounded to channel spacing
            virtual void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency) override;

            //! Is active frequency within 8.3383kHz channel?
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing8_33KHz);
            }

            //! Is active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return isWithinChannelSpacing(this->getFrequencyActive(), comFrequency, ChannelSpacing25KHz);
            }

            //! Set UNICOM frequency as active
            void setActiveUnicom()
            {
                this->toggleActiveStandby();
                this->setFrequencyActive(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
            }

            //! Set International Air Distress 121.5MHz
            void setActiveInternationalAirDistress()
            {
                this->toggleActiveStandby();
                this->setFrequencyActive(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress());
            }

            //! COM1 unit
            static CComSystem getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom1(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! COM1 unit
            static CComSystem getCom1System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom1(), activeFrequency, standbyFrequency == CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! COM2 unit
            static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1)
            {
                return CComSystem(CModulator::NameCom2(), BlackMisc::PhysicalQuantities::CFrequency(activeFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()), BlackMisc::PhysicalQuantities::CFrequency(standbyFrequencyMHz < 0 ? activeFrequencyMHz : standbyFrequencyMHz, BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz()));
            }

            //! COM2 unit
            static CComSystem getCom2System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet())
            {
                return CComSystem(CModulator::NameCom2(), activeFrequency, standbyFrequency ==  CModulator::FrequencyNotSet() ? activeFrequency : standbyFrequency);
            }

            //! Valid civil aviation frequency?
            static bool isValidCivilAviationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                if (f.isNull()) return false;
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 118.0 && fr <= 136.975;
            }

            //! Valid military aviation frequency?
            static bool isValidMilitaryFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                if (f.isNull()) return false;
                double fr = f.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::MHz(), 3);
                return fr >= 220.0 && fr <= 399.95;
            }

            //! Valid COM frequency (either civil or military)
            static bool isValidComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f)
            {
                return isValidCivilAviationFrequency(f) || isValidMilitaryFrequency(f);
            }

            /*!
             * Round to channel spacing, set MHz as unit
             * \see ChannelSpacing
             */
            static void roundToChannelSpacing(BlackMisc::PhysicalQuantities::CFrequency &frequency, ChannelSpacing channelSpacing);

            //! Is compareFrequency within channel spacing of setFrequency
            static bool isWithinChannelSpacing(const BlackMisc::PhysicalQuantities::CFrequency &setFrequency, const BlackMisc::PhysicalQuantities::CFrequency &compareFrequency, ChannelSpacing channelSpacing);

        protected:
            //! \copydoc CAvionicsBase::validValues
            virtual bool validValues() const override;

        private:
            ChannelSpacing m_channelSpacing = ChannelSpacing25KHz; //!< channel spacing

            /*!
             * Give me channel spacing in KHz
             * \remarks Just a helper method, that is why no CFrequency is returned
             */
            static double channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing);

            BLACK_ENABLE_TUPLE_CONVERSION(CComSystem)

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CComSystem, (o.m_channelSpacing))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem)

#endif // guard
