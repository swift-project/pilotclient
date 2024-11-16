// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_LOGINMODE_H
#define SWIFT_MISC_NETWORK_LOGINMODE_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CLoginMode)

namespace swift::misc::network
{
    //! Value object encapsulating information about login mode
    class SWIFT_MISC_EXPORT CLoginMode : public CValueObject<CLoginMode>
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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        LoginMode m_loginMode = Pilot;

        SWIFT_METACLASS(
            CLoginMode,
            SWIFT_METAMEMBER(loginMode));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CLoginMode)

#endif // guard
