/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_FREEFUNCTIONS_H
#define BLACKMISC_FREEFUNCTIONS_H

#include <QDir> // for Q_INIT_RESOURCE
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

    /*!
     * \brief Register all relevant metadata in BlackMisc
     */
    void registerMetadata();

    /*!
     * \brief Init resources
     */
    void initResources();

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

} // BlackMisc

#endif // guard
