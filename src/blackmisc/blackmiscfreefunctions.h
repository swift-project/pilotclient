/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_FREEFUNCTIONS_H
#define BLACKMISC_FREEFUNCTIONS_H

#include "valueobject.h" // for qHash overload
#include <QDir> // for Q_INIT_RESOURCE
#include <QList>
#include <QVariant>
#include <QDBusArgument>

/*!
 * Workaround, to call initResource from namespace. Used in BlackMisc::initResources().
 * Q_INIT_RESOURCE adds resource, here the translation files.
 */
// cannot be declared within namespace, see docu
// hence BlackMisc::initResources() calls this inline function
inline void initBlackMiscResources() { Q_INIT_RESOURCE(blackmisc); }

/*!
 * Free functions in BlackMisc
 */
namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*!
         * \brief Register all metadata for PQs
         */
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
    }

    namespace Hardware
    {
        //! Register metadata for Hardware
        void registerMetadata();
    }

    //! Register all relevant metadata in BlackMisc
    void registerMetadata();

    /*!
     * Deals with the issue, that the BlackMisc metatypes do not always start with
     * the same id, but dependent on enabled Qt modules. E.g. with GUI enabled:
     * type: 1024 name: QPaintBufferCacheEntry
     * type: 1025 .... some classes I cannot foresee
     * type: 1027 name: BlackMisc::CValueMap
     *
     * This is important when marshalling Variants via DBus among different
     * binaries, as an offset has to be considered
     *
     * \return
     */
    int firstBlackMetaType();

    //! Init resources
    void initResources();

    //! Compare 2 QVariants
    bool equalQVariants(const QVariant &v1, const QVariant &v2);

    //! Compare QVariants
    int compareQVariants(const QVariant &v1, const QVariant &v2);

    /*!
     * \brief QVariant to QString, allows to stringify CValueObject
     * \param qv    QVariant containing any CValueObject
     * \param i18n
     * \return
     */
    QString qVariantToString(const QVariant &qv, bool i18n = false);

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

    /*!
     * \brief Display all user metatypes
     * \remarks Used in order to debug code, do not remove
     */
    void displayAllUserMetatypesTypes();

    /*!
     * \brief Calculate a single hash value based on a list of individual hash values
     * \param values
     * \param className   add a hash value for class name on top
     * \return
     */
    uint calculateHash(const QList<uint> &values, const char *className);

    //! Real heap size of an object
    size_t heapSizeOf(const QMetaType &type);

    //! Real heap size of an object
    size_t heapSizeOf(const QMetaObject &objectType);

} // BlackMisc

#endif // guard
