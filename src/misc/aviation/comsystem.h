// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_COMSYSTEM_H
#define SWIFT_MISC_AVIATION_COMSYSTEM_H

#include "misc/aviation/modulator.h"
#include "misc/metaclass.h"
#include "misc/pq/constants.h"
#include "misc/pq/frequency.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/pq/pqstring.h"
#include "misc/swiftmiscexport.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"

#include <QHash>
#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <tuple>

namespace swift::misc::aviation
{
    //! COM system (aka "radio")
    class SWIFT_MISC_EXPORT CComSystem :
        public CModulator<CComSystem>,
        public mixin::MetaType<CComSystem>,
        public mixin::JsonOperators<CComSystem>,
        public mixin::Index<CComSystem>
    {
    public:
        //! Base type
        using base_type = CModulator<CComSystem>;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CComSystem)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CComSystem)

        //! Channel spacing frequency
        enum ChannelSpacing
        {
            ChannelSpacing50KHz, //!< 50kHz
            ChannelSpacing25KHz, //!< 25kHz
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
        CComSystem(const QString &name, const physical_quantities::CFrequency &activeFrequency, const physical_quantities::CFrequency &standbyFrequency = { 0, physical_quantities::CFrequencyUnit::nullUnit() }) : CModulator(name, activeFrequency, standbyFrequency.isNull() ? activeFrequency : standbyFrequency)
        {}

        //! Set active frequency
        //! \remarks will be rounded to channel spacing
        void setFrequencyActiveMHz(double frequencyMHz);

        //! Set standby frequency
        //! \remarks will be rounded to channel spacing
        void setFrequencyStandbyMHz(double frequencyMHz);

        //! Set active frequency
        //! \remarks will be rounded to channel spacing
        void setFrequencyActive(const physical_quantities::CFrequency &frequency);

        //! Set active frequency
        //! \remarks will be rounded to channel spacing
        void setFrequencyStandby(const physical_quantities::CFrequency &frequency);

        //! Is active frequency the same frequency
        bool isActiveFrequencySameFrequency(const physical_quantities::CFrequency &comFrequency) const;

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
        static CComSystem getCom1System(const physical_quantities::CFrequency &activeFrequency,
                                        const physical_quantities::CFrequency &standbyFrequency = { 0, physical_quantities::CFrequencyUnit::nullUnit() });
        //! COM2 unit
        static CComSystem getCom2System(double activeFrequencyMHz, double standbyFrequencyMHz = -1);

        //! COM2 unit
        static CComSystem getCom2System(const physical_quantities::CFrequency &activeFrequency,
                                        const physical_quantities::CFrequency &standbyFrequency = { 0, physical_quantities::CFrequencyUnit::nullUnit() });

        //! Valid civil aviation frequency?
        static bool isValidCivilAviationFrequency(const physical_quantities::CFrequency &f);

        //! Valid military aviation frequency?
        static bool isValidMilitaryFrequency(const physical_quantities::CFrequency &f);

        //! Valid COM frequency (either civil or military)
        static bool isValidComFrequency(const physical_quantities::CFrequency &f);

        //! Round to channel spacing, set MHz as unit
        //! \see ChannelSpacing
        static void roundToChannelSpacing(physical_quantities::CFrequency &frequency,
                                          ChannelSpacing channelSpacing);

        //! Compare frequencies under consideration that on VATSIM
        //! frequencies .x20/.x25 and .x70/.x75 are the same
        static bool isSameFrequency(const physical_quantities::CFrequency &freq1,
                                    const physical_quantities::CFrequency &freq2);

        //! Is passed frequency in kHz a valid 8.33 channel. This does not check if
        //! the frequency is within the correct bounds.
        static bool isValid8_33kHzChannel(int fKHz);

        //! Round passed frequency in kHz to 8.33 frequency spacing
        static int round8_33kHzChannel(int fKHz);

        //! Is frequency a "new" 8.33 kHz frequency and not within 25 kHz spacing
        //! E.g. returns false for 122.825 but true for 118.305
        static bool isExclusiveWithin8_33kHzChannel(const physical_quantities::CFrequency &freq);

        //! Is frequency within 25 kHz frequency spacing
        static bool isWithin25kHzChannel(const physical_quantities::CFrequency &freq);

        //! Parses almost any shitty string to a valid COM frequency
        static physical_quantities::CFrequency parseComFrequency(const QString &input, physical_quantities::CPqString::SeparatorMode sep);

        //! \copydoc swift::misc::CValueObject::registerMetadata
        static void registerMetadata();

    private:
        ChannelSpacing m_channelSpacing = ChannelSpacing8_33KHz; //!< channel spacing

        //! Give me channel spacing in KHz
        //! \remarks Just a helper method, that is why no CFrequency is returned
        static double channelSpacingToFrequencyKHz(ChannelSpacing channelSpacing);

        SWIFT_METACLASS(
            CComSystem,
            SWIFT_METAMEMBER(channelSpacing));
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::aviation::CComSystem)
Q_DECLARE_METATYPE(swift::misc::aviation::CComSystem::ChannelSpacing)
Q_DECLARE_METATYPE(swift::misc::aviation::CComSystem::ComUnit)

#endif // guard
