/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_COMSYSTEM_H
#define BLACKMISC_AVIATION_COMSYSTEM_H

#include "blackmisc/aviation/modulator.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixinindex.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/mixin/mixinmetatype.h"

#include <QHash>
#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <tuple>

namespace BlackMisc::Aviation
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
            ChannelSpacing50KHz,  //!< 50kHz
            ChannelSpacing25KHz,  //!< 25kHz
            ChannelSpacing8_33KHz //!< 8.33kHz
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
        CComSystem(const QString &name, const PhysicalQuantities::CFrequency &activeFrequency, const PhysicalQuantities::CFrequency &standbyFrequency = { 0, PhysicalQuantities::CFrequencyUnit::nullUnit() }):
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
        void setFrequencyActive(const PhysicalQuantities::CFrequency &frequency);

        //! Set active frequency
        //! \remarks will be rounded to channel spacing
        void setFrequencyStandby(const PhysicalQuantities::CFrequency &frequency);

        //! Is active frequency within 8.3383kHz channel?
        bool isActiveFrequencyWithin8_33kHzChannel(const PhysicalQuantities::CFrequency &comFrequency) const;

        //! Is active frequency within 25kHz channel?
        bool isActiveFrequencyWithin25kHzChannel(const PhysicalQuantities::CFrequency &comFrequency) const;

        //! Is active frequency within 25kHz channel?
        bool isActiveFrequencyWithin50kHzChannel(const PhysicalQuantities::CFrequency &comFrequency) const;

        //! Is active frequency within the channel spacing?
        bool isActiveFrequencyWithinChannelSpacing(const PhysicalQuantities::CFrequency &comFrequency) const;

        //! Set UNICOM frequency as active
        void setActiveUnicom();

        //! Set International Air Distress 121.5MHz
        void setActiveInternationalAirDistress();

        //! Get channel spacing
        ChannelSpacing getChannelSpacing() const { return m_channelSpacing; }

        //! Set channel spacing
        void setChannelSpacing(ChannelSpacing spacing) { m_channelSpacing = spacing; }

        //! COM1 unit
        static CComSystem getCom1System(double activeFrequencyMHz, double standbyFrequencyMHz = -1);

        //! COM1 unit
        static CComSystem getCom1System(const PhysicalQuantities::CFrequency &activeFrequency,
                                        const PhysicalQuantities::CFrequency &standbyFrequency = { 0, PhysicalQuantities::CFrequencyUnit::nullUnit() });
        //! COM2 unit
        static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1);

        //! COM2 unit
        static CComSystem getCom2System(const PhysicalQuantities::CFrequency &activeFrequency,
                                        const PhysicalQuantities::CFrequency &standbyFrequency = { 0, PhysicalQuantities::CFrequencyUnit::nullUnit() });

        //! Valid civil aviation frequency?
        static bool isValidCivilAviationFrequency(const PhysicalQuantities::CFrequency &f);

        //! Valid military aviation frequency?
        static bool isValidMilitaryFrequency(const PhysicalQuantities::CFrequency &f);

        //! Valid COM frequency (either civil or military)
        static bool isValidComFrequency(const PhysicalQuantities::CFrequency &f);

        //! Round to channel spacing, set MHz as unit
        //! \see ChannelSpacing
        static void roundToChannelSpacing(PhysicalQuantities::CFrequency &frequency,
                                            ChannelSpacing channelSpacing);

        //! Is compareFrequency within channel spacing of setFrequency
        static bool isWithinChannelSpacing(const PhysicalQuantities::CFrequency &setFrequency,
                                            const PhysicalQuantities::CFrequency &compareFrequency,
                                            ChannelSpacing channelSpacing);

        //! Is passed frequency in kHz a valid 8.33 channel. This does not check if
        //! the frequency is within the correct bounds.
        static bool isValid8_33kHzChannel(int fKHz);

        //! Round passed frequency in kHz to 8.33 frequency spacing
        static int round8_33kHzChannel(int fKHz);

        //! Parses almost any shitty string to a valid COM frequency
        static PhysicalQuantities::CFrequency parseComFrequency(const QString &input, PhysicalQuantities::CPqString::SeparatorMode sep);

        //! \copydoc BlackMisc::CValueObject::registerMetadata
        static void registerMetadata();

    private:
        ChannelSpacing m_channelSpacing = ChannelSpacing8_33KHz; //!< channel spacing

        //! Give me channel spacing in KHz
        //! \remarks Just a helper method, that is why no CFrequency is returned
        static double channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing);

        BLACK_METACLASS(
            CComSystem,
            BLACK_METAMEMBER(channelSpacing)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem::ChannelSpacing)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CComSystem::ComUnit)

#endif // guard
