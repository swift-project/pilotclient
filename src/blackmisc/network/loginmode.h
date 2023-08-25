// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_LOGINMODE_H
#define BLACKMISC_NETWORK_LOGINMODE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CLoginMode)

namespace BlackMisc::Network
{
    //! Value object encapsulating information about login mode
    class BLACKMISC_EXPORT CLoginMode : public CValueObject<CLoginMode>
    {
    public:
        //! Login modes
        enum LoginMode
        {
            Pilot, //!< Normal login
            Observer, //!< Login as observer
        };

        //! Default constructor.
        CLoginMode() = default;

        //! Constructor
        CLoginMode(LoginMode mode) : m_loginMode(mode) {}

        //! Is login as pilot?
        bool isPilot() const { return m_loginMode == Pilot; }

        //! Is login as observer?
        bool isObserver() const { return m_loginMode == Observer; }

        //! Get login mode
        LoginMode getLoginMode() const { return m_loginMode; }

        //! Set login mode
        void setLoginMode(LoginMode mode) { m_loginMode = mode; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        LoginMode m_loginMode = Pilot;

        BLACK_METACLASS(
            CLoginMode,
            BLACK_METAMEMBER(loginMode)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CLoginMode)

#endif // guard
