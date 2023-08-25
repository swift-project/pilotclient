// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTENGINES_H
#define BLACKMISC_AVIATION_AIRCRAFTENGINES_H

#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftEngine)

namespace BlackMisc::Aviation
{
    //! Value object encapsulating information about aircraft's engines
    class BLACKMISC_EXPORT CAircraftEngine : public CValueObject<CAircraftEngine>
    {
    public:
        //! Default constructor
        CAircraftEngine() {}

        //! Constructor
        //! \remark numbers are 1 based!
        CAircraftEngine(int number, bool on);

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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        int m_number = 1;
        bool m_on = true;

        BLACK_METACLASS(
            CAircraftEngine,
            BLACK_METAMEMBER(number, 0, DisabledForJson),
            BLACK_METAMEMBER(on)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftEngine)

#endif // guard
