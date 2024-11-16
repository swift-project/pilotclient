// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_DBUSERROR_H
#define SWIFT_SIM_XSWIFTBUS_DBUSERROR_H

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

} // namespace XSwiftBus

#endif // guard
