// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTENGINES_H
#define SWIFT_MISC_AVIATION_AIRCRAFTENGINES_H

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftEngine)

namespace swift::misc::aviation
{
    //! Value object encapsulating information about aircraft's engines
    class SWIFT_MISC_EXPORT CAircraftEngine : public CValueObject<CAircraftEngine>
    {
    public:
        //! Default constructor
        CAircraftEngine() = default;

        //! Constructor
        //! \remark numbers are 1 based!
        // CAircraftEngine(int number, bool on);

        CAircraftEngine(int number, bool on, int enginePower);

        //! Get engine number
        //! \remark numbers are 1 based!
        int getNumber() const { return m_number; }

        //! Set engine number
        //! \remark numbers are 1 based!
        void setNumber(int number);

        //! Is on/off?
        bool isOn() const { return m_on; }

        //! Set to on/off
        void setOn(bool on) { m_on = on; }

        //! Set engine percentage (0..100)
        void setEnginePower(double percentage);
        double getEnginePower() const { return m_power; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        int m_number = 1;
        bool m_on = false;
        double m_power = 0.0;

        SWIFT_METACLASS(
            CAircraftEngine,
            SWIFT_METAMEMBER(number, 0, DisabledForJson),
            SWIFT_METAMEMBER(on),
            SWIFT_METAMEMBER(power));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftEngine)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTENGINES_H
