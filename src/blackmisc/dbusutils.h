/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DBUSUTILS_H
#define BLACKMISC_DBUSUTILS_H

#include "blackmisc/blackmiscexport.h"
#include <QDBusArgument>
#include <QTextStream>

namespace BlackMisc
{
    //! Utils for UNIT tests / samples
    class BLACKMISC_EXPORT CDBusUtils
    {
    public:
        //! Get QDBusArgument signature (formatted)
        static QString getQDBusArgumentSignature(const QDBusArgument &arg, int level = 0);

        //! Signature for BlackMisc::CValueObject
        template <typename ValueObj>
        static QString dBusSignature(const ValueObj &obj)
        {
            QDBusArgument arg;
            obj.marshallToDbus(arg);
            return arg.currentSignature();
        }

        //! Signature size for BlackMisc::CValueObject
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
} // ns

#endif // guard
