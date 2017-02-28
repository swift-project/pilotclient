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

#include "blackmisc/aviation/modulator.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/json.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variant.h"

#include <QHash>
#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        //! COM system (aka "radio")
        class BLACKMISC_EXPORT CComSystem :
            public CModulator<CComSystem>,
            public Mixin::MetaType<CComSystem>,
            public Mixin::JsonOperators<CComSystem>,
            public Mixin::Index<CComSystem>
        {
        public:
            //! Base type
            using base_type = CModulator<CComSystem>;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CComSystem)
            BLACKMISC_DECLARE_USING_MIXIN_INDEX(CComSystem)

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
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const;

            //! Is active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const;

            //! Set UNICOM frequency as active
            void setActiveUnicom();

            //! Set International Air Distress 121.5MHz
            void setActiveInternationalAirDistress();

            //! COM1 unit
            static CComSystem getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1);

            //! COM1 unit
            static CComSystem getCom1System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet());

            //! COM2 unit
            static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1);

            //! COM2 unit
            static CComSystem getCom2System(BlackMisc::PhysicalQuantities::CFrequency activeFrequency, BlackMisc::PhysicalQuantities::CFrequency standbyFrequency = CModulator::FrequencyNotSet());

            //! Valid civil aviation frequency?
            static bool isValidCivilAviationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f);

            //! Valid military aviation frequency?
            static bool isValidMilitaryFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f);

            //! Valid COM frequency (either civil or military)
            static bool isValidComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f);

            //! Round to channel spacing, set MHz as unit
            //! \see ChannelSpacing
            static void roundToChannelSpacing(BlackMisc::PhysicalQuantities::CFrequency &frequency, ChannelSpacing channelSpacing);

            //! Is compareFrequency within channel spacing of setFrequency
            static bool isWithinChannelSpacing(const BlackMisc::PhysicalQuantities::CFrequency &setFrequency, const BlackMisc::PhysicalQuantities::CFrequency &compareFrequency, ChannelSpacing channelSpacing);

            //! \copydoc BlackMisc::CValueObject::registerMetadata
            static void registerMetadata();

        protected:
            //! \copydoc CModulator::validValues
            virtual bool validValues() const override;

        private:
            ChannelSpacing m_channelSpacing = ChannelSpacing25KHz; //!< channel spacing

            //! Give me channel spacing in KHz
            //! \remarks Just a helper method, that is why no CFrequency is returned
            static double channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing);

            BLACK_METACLASS(
                CComSystem,
                BLACK_METAMEMBER(channelSpacing)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem::ChannelSpacing)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem::ComUnit)

#endif // guard
