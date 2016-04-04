/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STRINGUTILS_H
#define BLACKMISC_STRINGUTILS_H

#include "blackmisc/blackmiscexport.h"
#include <QMap>
#include <QByteArray>
#include <QString>
#include <QDebug>

//! Free functions in BlackMisc
namespace BlackMisc
{
    //! A map converted to string
    template<class K, class V> QString qmapToString(const QMap<K, V> &map)
    {
        QString s;
        const QString kv("%1: %2 ");
        QMapIterator<K, V> i(map);
        while (i.hasNext())
        {
            i.next();
            s.append(
                kv.arg(i.key()).arg(i.value())
            );
        }
        return s.trimmed();
    }

    //! Bool to on/off
    BLACKMISC_EXPORT QString boolToOnOff(bool v, bool  i18n = false);

    //! Bool to yes / no
    BLACKMISC_EXPORT QString boolToYesNo(bool v, bool  i18n = false);

    //! Bool to true / false
    BLACKMISC_EXPORT QString boolToTrueFalse(bool v, bool  i18n = false);

    //! Convert string to bool
    BLACKMISC_EXPORT bool stringToBool(const QString &boolString);

    //! Int to hex value
    BLACKMISC_EXPORT QString intToHex(int value, int digits = 2);

    //! Int to hex value (per byte, 2 digits)
    BLACKMISC_EXPORT QString bytesToHexString(const QByteArray &bytes);

    //! Byte array from hex value string per byte, 2 digits
    BLACKMISC_EXPORT QByteArray byteArrayFromHexString(const QString &hexString);

    //! Strip a designator from a combined string
    BLACKMISC_EXPORT QString stripDesignatorFromCompleterString(const QString &candidate);

    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit string streaming operations.
         *
         * \tparam Derived Must implement a public method QString convertToQString(bool i18n = false) const.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_STRING
         */
        template <class Derived>
        class String
        {
        public:
            //! Stream << overload to be used in debugging messages
            friend QDebug operator<<(QDebug debug, const Derived &obj)
            {
                debug << obj.stringForStreaming();
                return debug;
            }

            //! Operator << when there is no debug stream
            friend QNoDebug operator<<(QNoDebug nodebug, const Derived &obj)
            {
                Q_UNUSED(obj);
                return nodebug;
            }

            //! Operator << based on text stream
            friend QTextStream &operator<<(QTextStream &stream, const Derived &obj)
            {
                stream << obj.stringForStreaming();
                return stream;
            }

            //! Operator << for QDataStream
            friend QDataStream &operator<<(QDataStream &stream, const Derived &obj)
            {
                stream << obj.stringForStreaming();
                return stream;
            }

            //! Stream operator << for std::cout
            friend std::ostream &operator<<(std::ostream &ostr, const Derived &obj)
            {
                ostr << obj.stringForStreaming().toStdString();
                return ostr;
            }

            //! Cast as QString
            QString toQString(bool i18n = false) const { return derived()->convertToQString(i18n); }

            //! Cast to pretty-printed QString
            QString toFormattedQString(bool i18n = false) const { return derived()->toQString(i18n); }

            //! To std string
            std::string toStdString(bool i18n = false) const { return derived()->convertToQString(i18n).toStdString(); }

            //! String for streaming operators
            QString stringForStreaming() const { return derived()->convertToQString(); }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::String,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_STRING(DERIVED)                \
            using ::BlackMisc::Mixin::String<DERIVED>::toQString;           \
            using ::BlackMisc::Mixin::String<DERIVED>::toFormattedQString;  \
            using ::BlackMisc::Mixin::String<DERIVED>::toStdString;         \
            using ::BlackMisc::Mixin::String<DERIVED>::stringForStreaming;
    } // ns
} // ns

#endif // guard
