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
    /*!
     * Free functions in PQs
     */
    namespace PhysicalQuantities
    {

        /*!
         * \brief Register all metadata for PQs
         */
        void registerMetadata();

    } // PQ

    /*!
     * Free functions in aviation
     */
    namespace Aviation
    {

        /*!
         * \brief Register metadata for aviation
         */
        void registerMetadata();

    } // Aviation

    /*!
     * Free functions in Math
     */
    namespace Math
    {
        /*!
         * \brief Register metadata for math (matrices, vectors)
         */
        void registerMetadata();

    } // Math

    namespace Geo
    {

        /*!
         * \brief Register metadata for geo (coordinates)
         */
        void registerMetadata();

    } // Geo

    namespace Network
    {
        /*!
         * \brief Register metadata for network (server, user)
         */
        void registerMetadata();
    }

    namespace Settings
    {
        /*!
         * \brief Register metadata for settings
         */
        void registerMetadata();
    }

    /*!
     * \brief Register all relevant metadata in BlackMisc
     */
    void registerMetadata();

    /*!
     * Deals with the issue that the BlackMisc metatype does not always start with
     * the same id, e.g. with GUI enabled:
     * type: 1024 name: QPaintBufferCacheEntry
     * type: 1025 .... some classes I cannot foresee
     * type: 1027 name: BlackMisc::CValueMap
     *
     * This is important when marshalling Variants via DBus among different
     * binaries, as the offset has to be considered
     *
     * \return
     */
    int firstBlackMetaType();

    /*!
     * \brief Init resources
     */
    void initResources();

    /*!
     * \brief Compare 2 QVariants
     * \param v1
     * \param v2
     */
    bool equalQVariants(const QVariant &v1, const QVariant &v2);

    /*!
     * \brief Compare QVariants
     * \param v1
     * \param v2
     * \return
     */
    int compareQVariants(const QVariant &v1, const QVariant &v2);

    /*!
     * \brief QVariant to string, allows to stringify CValueObject
     * \param qv
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
     * received and Qt is nor able to get the original QVariant back
     * \param argument
     * \param type
     * \return
     */
    // TODO: To be removed if a better solution is found
    QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type);

    /*!
     * \brief displayAllUserTypes
     */
    void displayAllUserMetatypesTypes();

    /*!
     * \brief Add several hash values
     * \param values
     * \param classTypeId
     * \return
     */
    uint calculateHash(const QList<uint> &values, const char *className);

    /*!
     * \brief Real heap size of an object
     * \param type
     * \return
     */
    size_t heapSizeOf(const QMetaType &type);

    /*!
     * \brief Real heap size of an object
     * \param objectType
     * \return
     */
    size_t heapSizeOf(const QMetaObject &objectType);

} // BlackMisc

#endif // guard
