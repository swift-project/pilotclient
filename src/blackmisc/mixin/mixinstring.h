// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_MIXIN_MIXINSTRING_H
#define BLACKMISC_MIXIN_MIXINSTRING_H

#include <QByteArray>
#include <QDebug>
#include <QString>
#include <QStringRef>
#include <QStringView>
#include <QTextStream>
#include <QtGlobal>
#include <string>
#include <ostream>

namespace BlackMisc::Mixin
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
            Q_UNUSED(obj)
            return nodebug;
        }

        //! Operator << based on text stream
        friend QTextStream &operator<<(QTextStream &stream, const Derived &obj)
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
        QString toQString(bool i18n = false) const;

        //! To std string
        std::string toStdString(bool i18n = false) const;

        //! String for streaming operators
        QString stringForStreaming() const;

    private:
        const Derived *derived() const;
        Derived *derived();
    };

    template <class Derived>
    QString String<Derived>::toQString(bool i18n) const
    {
        return derived()->convertToQString(i18n);
    }

    template <class Derived>
    std::string String<Derived>::toStdString(bool i18n) const
    {
        return derived()->convertToQString(i18n).toStdString();
    }

    template <class Derived>
    QString String<Derived>::stringForStreaming() const
    {
        return derived()->convertToQString();
    }

    template <class Derived>
    const Derived *String<Derived>::derived() const
    {
        return static_cast<const Derived *>(this);
    }

    template <class Derived>
    Derived *String<Derived>::derived()
    {
        return static_cast<Derived *>(this);
    }

    // *INDENT-OFF*
    /*!
     * When a derived class and a base class both inherit from Mixin::String,
     * the derived class uses this macro to disambiguate the inherited members.
     */
#define BLACKMISC_DECLARE_USING_MIXIN_STRING(DERIVED)       \
    using ::BlackMisc::Mixin::String<DERIVED>::toQString;   \
    using ::BlackMisc::Mixin::String<DERIVED>::toStdString; \
    using ::BlackMisc::Mixin::String<DERIVED>::stringForStreaming;
    // *INDENT-ON*
} // ns

#endif // guard
