// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DBUSUTILS_H
#define SWIFT_MISC_DBUSUTILS_H

#include <QDBusArgument>
#include <QTextStream>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Utils for UNIT tests / samples
    class SWIFT_MISC_EXPORT CDBusUtils
    {
    public:
        //! Get QDBusArgument signature (formatted)
        static QString getQDBusArgumentSignature(const QDBusArgument &arg, int level = 0);

        //! Signature for swift::misc::CValueObject
        template <typename ValueObj>
        static QString dBusSignature(const ValueObj &obj)
        {
            QDBusArgument arg;
            obj.marshallToDbus(arg);
            return arg.currentSignature();
        }

        //! Signature size for swift::misc::CValueObject
        template <typename ValueObj>
        static int dBusSignatureSize(const ValueObj &obj)
        {
            return dBusSignature<ValueObj>(obj).size();
        }

        //! Type as string
        static QString dbusTypeAsString(QDBusArgument::ElementType type);

        //! Show some (important) DBus signatures
        static void showDBusSignatures(QTextStream &out);
    };
} // namespace swift::misc

#endif // guard
