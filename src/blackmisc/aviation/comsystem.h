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
            CComSystem(const QString &name, const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency, const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() }):
                CModulator(name, activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency)
            { }

            //! Set active frequency
            //! \remarks will be rounded to channel spacing
            void setFrequencyActiveMHz(double frequencyMHz);

            //! Set standby frequency
            //! \remarks will be rounded to channel spacing
            void setFrequencyStandbyMHz(double frequencyMHz);

            //! Set active frequency
            //! \remarks will be rounded to channel spacing
            void setFrequencyActive(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set active frequency
            //! \remarks will be rounded to channel spacing
            void setFrequencyStandby(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

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
            static CComSystem getCom1System(const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency,
                                            const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() });

            //! COM2 unit
            static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1);

            //! COM2 unit
            static CComSystem getCom2System(const BlackMisc::PhysicalQuantities::CFrequency &activeFrequency,
                                            const BlackMisc::PhysicalQuantities::CFrequency &standbyFrequency = { 0, BlackMisc::PhysicalQuantities::CFrequencyUnit::nullUnit() });

            //! Valid civil aviation frequency?
            static bool isValidCivilAviationFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f);

            //! Valid military aviation frequency?
            static bool isValidMilitaryFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f);

            //! Valid COM frequency (either civil or military)
            static bool isValidComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &f);

            //! Round to channel spacing, set MHz as unit
            //! \see ChannelSpacing
            static void roundToChannelSpacing(BlackMisc::PhysicalQuantities::CFrequency &frequency,
                                              ChannelSpacing channelSpacing);

            //! Is compareFrequency within channel spacing of setFrequency
            static bool isWithinChannelSpacing(const BlackMisc::PhysicalQuantities::CFrequency &setFrequency,
                                               const BlackMisc::PhysicalQuantities::CFrequency &compareFrequency,
                                               ChannelSpacing channelSpacing);

            //! \copydoc BlackMisc::CValueObject::registerMetadata
            static void registerMetadata();

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
