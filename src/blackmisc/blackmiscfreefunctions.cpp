/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmiscfreefunctions.h"
#include "avallclasses.h"
#include "pqallquantities.h"
#include "mathallclasses.h"
#include "geoallclasses.h"

/*
 * Metadata for PQs
 */
void BlackMisc::PhysicalQuantities::registerMetadata()
{
    CAcceleration::registerMetadata();
    CAngle::registerMetadata();
    CFrequency::registerMetadata();
    CLength::registerMetadata();
    CMass::registerMetadata();
    CPressure::registerMetadata();
    CSpeed::registerMetadata();
    CTemperature::registerMetadata();
    CTime::registerMetadata();
}

/*
 * Metadata for aviation
 */
void BlackMisc::Aviation::registerMetadata()
{
    CComSystem::registerMetadata();
    CNavSystem::registerMetadata();
    CAdfSystem::registerMetadata();
    CAltitude::registerMetadata();
    CTransponder::registerMetadata();
    CHeading::registerMetadata();
    CTrack::registerMetadata();
}

/*
 * Metadata for Math
 */
void BlackMisc::Math::registerMetadata()
{
    CMatrix3x3::registerMetadata();
    CMatrix3x1::registerMetadata();
    CMatrix1x3::registerMetadata();
    CVector3D::registerMetadata();
}


/*
 * Metadata for Geo
 */
void BlackMisc::Geo::registerMetadata()
{
    CCoordinateEcef::registerMetadata();
    CCoordinateNed::registerMetadata();
    CCoordinateGeodetic::registerMetadata();
    CLatitude::registerMetadata();
    CLongitude::registerMetadata();
}

/*
 * Metadata for Blackmisc
 */
void BlackMisc::registerMetadata()
{
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
    Math::registerMetadata();
    Geo::registerMetadata();
}

/*
 * Init resources
 */
void BlackMisc::initResources()
{
    initBlackMiscResources();
}
/*
 * To string
 */
QString BlackMisc::qVariantToString(const QVariant &qv, bool i18n)
{
    if (qv.type() != QVariant::UserType) return qv.toString();
    const CValueObject *s = CValueObject::fromQVariant(qv);
    if (s)
    {
        return s->toQString(i18n);
    }
    else
    {
        return "unknown";
    }
}

/*
 * Fix QVariant if it comes from DBus and contains QDBusArgument
 */
QVariant BlackMisc::fixQVariantFromDbusArgument(const QVariant &variant, int localUserType)
{
    if (variant.canConvert<QDBusArgument>())
    {
        // complex, user type
        // it has to be made sure, that the cast works
        const QDBusArgument arg = variant.value<QDBusArgument>();
        QVariant fixedVariant;
        if (localUserType < static_cast<int>(QVariant::UserType))
        {
            // complex Qt type, e.g. QDateTime
            fixedVariant = BlackMisc::complexQtTypeFromDbusArgument(arg, localUserType);
        }
        else
        {
            // http://qt-project.org/doc/qt-5.0/qtcore/qmetatype.html#create
            void *obByMetaId = QMetaType::create(localUserType);

            // own types, send as QDBusArgument
            CValueObject *streamable = static_cast<CValueObject *>(obByMetaId);
            arg >> (*streamable);
            fixedVariant = streamable->toQVariant();
            QMetaType::destroy(localUserType, obByMetaId);
        }
        return fixedVariant;
    }
    else
    {
        return variant;
    }
}

/*
 * Return QVariant based on QDBusArgument
 */
QVariant BlackMisc::complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type)
{
    // QDate = 14, QTime = 15, QDateTime = 16, QUrl = 17,

    switch (type)
    {
    case QMetaType::QDateTime:
        {
            QDateTime dt;
            argument >> dt;
            return QVariant::fromValue(dt);
        }
    case QMetaType::QDate:
        {
            QDate date;
            argument >> date;
            return QVariant::fromValue(date);
        }
    case QMetaType::QTime:
        {
            QTime time;
            argument >> time;
            return QVariant::fromValue(time);
        }
    default:
        qFatal("Type cannot be resolved");
    }
    return QVariant(); // suppress compiler warning
}

