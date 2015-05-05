/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_FREEFUNCTIONS_H
#define BLACKMISC_FREEFUNCTIONS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/tuple.h"
#include "blackmisc/inheritance_traits.h"
#include <QDir> // for Q_INIT_RESOURCE
#include <QList>
#include <QMap>
#include <QVariant>
#include <QMetaType>
#include <QDBusArgument>
#include <QTextStream>

#include <memory>

/*!
 * Workaround, to call initResource from namespace. Used in BlackMisc::initResources().
 * Q_INIT_RESOURCE adds resource, here the translation files.
 */
inline void initBlackMiscResources()
{
    // cannot be declared within namespace, see docu
    // hence BlackMisc::initResources() calls this inline function
    Q_INIT_RESOURCE(blackmisc);
}

//! Free functions in BlackMisc
namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        //! Register all metadata for PQs
        BLACKMISC_EXPORT void registerMetadata();

    } // PQ

    namespace Aviation
    {
        //! Register metadata for aviation
        BLACKMISC_EXPORT void registerMetadata();

    } // Aviation

    namespace Math
    {
        //! Register metadata for math (matrices, vectors)
        BLACKMISC_EXPORT void registerMetadata();

    } // Math

    namespace Geo
    {

        //! Register metadata for geo (coordinates)
        BLACKMISC_EXPORT void registerMetadata();

    } // Geo

    namespace Network
    {
        //! Register metadata for network (server, user)
        BLACKMISC_EXPORT void registerMetadata();
    }

    namespace Settings
    {
        //! Register metadata for settings
        BLACKMISC_EXPORT void registerMetadata();
    }

    namespace Audio
    {
        //! Register metadata for audio / voice
        BLACKMISC_EXPORT void registerMetadata();

        //! Start the Windows mixer
        BLACKMISC_EXPORT bool startWindowsMixer();
    }

    namespace Hardware
    {
        //! Register metadata for Hardware
        BLACKMISC_EXPORT void registerMetadata();
    }

    namespace Event
    {
        //! Register metadata for Event
        BLACKMISC_EXPORT void registerMetadata();
    }

    namespace Simulation
    {
        //! Register metadata for Simulation
        BLACKMISC_EXPORT void registerMetadata();
    }

    //! Register all relevant metadata in BlackMisc
    BLACKMISC_EXPORT void registerMetadata();

    //! Init resources
    BLACKMISC_EXPORT void initResources();

    namespace Mixin
    {

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with hashing instances by metatuple.
         *
         * \tparam Derived Must be registered with BLACK_DECLARE_TUPLE_CONVERSION.
         */
        template <class Derived>
        class HashByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! qHash overload, needed for storing value in a QSet.
            friend uint qHash(const Derived &value, uint seed = 0)
            {
                return ::qHash(hashImpl(value), seed);
            }

        private:
            static uint hashImpl(const Derived &value)
            {
                return BlackMisc::qHash(toMetaTuple(value)) ^ baseHash(static_cast<const BaseOfT<Derived> *>(&value));
            }

            template <typename T> static uint baseHash(const T *base) { return qHash(*base); }
            static uint baseHash(const void *) { return 0; }
        };

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

    } // Mixin

    //! Checked version from QVariant
    template <class T> void setFromQVariant(T *value, const QVariant &variant)
    {
        if (variant.canConvert<T>())
        {
            (*value) = variant.value<T>();
        }
        else
        {
            const QString m = QString("Target type: %1 Variant type: %2 %3 %4").
                              arg(qMetaTypeId<T>()).
                              arg(static_cast<int>(variant.type())).arg(variant.userType()).arg(variant.typeName());
            Q_ASSERT_X(false, "setFromQVariant", m.toLocal8Bit().constData());
            Q_UNUSED(m);
        }
    }

    /*!
     * Fix variant. Required if a variant "comes in" via DBus,
     * and represents a QDBusArgument rather than the real type.
     * \param variant
     * \param localUserType
     * \return
     */
    BLACKMISC_EXPORT QVariant fixQVariantFromDbusArgument(const QVariant &variant, int localUserType);

    /*!
     * Convert a QDBusArgument back to a concrete type if required
     * This is a workaround if a QVariant is sent via DBus, QDBusArgument is
     * received and Qt is not able to get the original QVariant back
     * \param argument  send via DBus
     * \param type      Metatype
     * \return          QVariant generated from the above
     * \todo To be removed if a better solution is found
     */
    BLACKMISC_EXPORT QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type);

    //! Display all user metatypes
    //! \remarks Used in order to debug code, do not remove
    BLACKMISC_EXPORT void displayAllUserMetatypesTypes(QTextStream &out);

    /*!
     * \brief Calculate a single hash value based on a list of individual hash values
     * \param values
     * \param className   add a hash value for class name on top
     * \return
     */
    BLACKMISC_EXPORT uint calculateHash(const QList<uint> &values, const char *className);

    //! Hash value, but with int list
    BLACKMISC_EXPORT uint calculateHash(const QList<int> &values, const char *className);

    //! Real heap size of an object
    BLACKMISC_EXPORT size_t heapSizeOf(const QMetaType &type);

    //! Real heap size of an object
    BLACKMISC_EXPORT size_t heapSizeOf(const QMetaObject &objectType);

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

    //! Get local host name
    BLACKMISC_EXPORT const QString &localHostName();

    //! Get local host name env.variable
    BLACKMISC_EXPORT const QString &localHostNameEnvVariable();

    //! Own implementation of std::make_unique, a C++14 feature not provided by GCC in C++11 mode
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    //! Creates an incremental json object from two existing objects
    BLACKMISC_EXPORT QJsonObject getIncrementalObject(const QJsonObject &previousObject, const QJsonObject &currentObject);

    //! Merges an incremental json object into an existing one
    BLACKMISC_EXPORT QJsonObject applyIncrementalObject(const QJsonObject &previousObject, const QJsonObject &incrementalObject);



} // BlackMisc

#endif // guard
