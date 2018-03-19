/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_DBUSERROR_H
#define BLACKSIM_XSWIFTBUS_DBUSERROR_H

#include <dbus/dbus.h>
#include <string>

namespace XSwiftBus
{

    //! DBus error
    class CDBusError
    {
    public:
        //! Error type
        enum ErrorType
        {
            NoError,
            Other
        };

        //! Default constructur
        CDBusError() = default;

        //! Constructor
        explicit CDBusError(const DBusError *error);

        //! Get error type
        ErrorType getType() const { return m_errorType; }

    private:
        ErrorType m_errorType = NoError;
        std::string m_name;
        std::string m_message;
    };

}

#endif // guard
