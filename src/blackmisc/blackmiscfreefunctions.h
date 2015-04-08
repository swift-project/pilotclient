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
        void registerMetadata();

    } // PQ

    namespace Aviation
    {
        //! Register metadata for aviation
        void registerMetadata();

    } // Aviation

    namespace Math
    {
        //! Register metadata for math (matrices, vectors)
        void registerMetadata();

    } // Math

    namespace Geo
    {

        //! Register metadata for geo (coordinates)
        void registerMetadata();

    } // Geo

    namespace Network
    {
        //! Register metadata for network (server, user)
        void registerMetadata();
    }

    namespace Settings
    {
        //! Register metadata for settings
        void registerMetadata();
    }

    namespace Audio
    {
        //! Register metadata for audio / voice
        void registerMetadata();

        //! Start the Windows mixer
        bool startWindowsMixer();
    }

    namespace Hardware
    {
        //! Register metadata for Hardware
        void registerMetadata();
    }

    namespace Event
    {
        //! Register metadata for Event
        void registerMetadata();
    }

    namespace Simulation
    {
        //! Register metadata for Simulation
        void registerMetadata();
    }

    //! Register all relevant metadata in BlackMisc
    void registerMetadata();

    //! Init resources
    void initResources();

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
    QVariant fixQVariantFromDbusArgument(const QVariant &variant, int localUserType);

    /*!
     * Convert a QDBusArgument back to a concrete type if required
     * This is a workaround if a QVariant is sent via DBus, QDBusArgument is
     * received and Qt is not able to get the original QVariant back
     * \param argument  send via DBus
     * \param type      Metatype
     * \return          QVariant generated from the above
     * \todo To be removed if a better solution is found
     */
    QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type);

    //! Display all user metatypes
    //! \remarks Used in order to debug code, do not remove
    void displayAllUserMetatypesTypes(QTextStream &out);

    /*!
     * \brief Calculate a single hash value based on a list of individual hash values
     * \param values
     * \param className   add a hash value for class name on top
     * \return
     */
    uint calculateHash(const QList<uint> &values, const char *className);

    //! Hash value, but with int list
    uint calculateHash(const QList<int> &values, const char *className);

    //! Real heap size of an object
    size_t heapSizeOf(const QMetaType &type);

    //! Real heap size of an object
    size_t heapSizeOf(const QMetaObject &objectType);

    //! A map converted to string
    template<class K, class V> QString qmapToString(const QMap<K, V> &map);

    //! Bool to on/off
    QString boolToOnOff(bool v, bool  i18n = false);

    //! Bool to yes / no
    QString boolToYesNo(bool v, bool  i18n = false);

    //! Bool to true / false
    QString boolToTrueFalse(bool v, bool  i18n = false);

    //! Get local host name
    const QString &localHostName();

    //! Get local host name env.variable
    const QString &localHostNameEnvVariable();

    //! Own implementation of std::make_unique, a C++14 feature not provided by GCC in C++11 mode
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    //! Creates an incremental json object from two existing objects
    QJsonObject getIncrementalObject(const QJsonObject &previousObject, const QJsonObject &currentObject);

    //! Merges an incremental json object into an existing one
    QJsonObject applyIncrementalObject(const QJsonObject &previousObject, const QJsonObject &incrementalObject);



} // BlackMisc

#endif // guard
