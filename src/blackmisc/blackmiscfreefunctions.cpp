/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmiscfreefunctions.h"
#include "avallclasses.h"
#include "pqallquantities.h"
#include "mathallclasses.h"
#include "geoallclasses.h"
#include "networkallclasses.h"
#include "settingsallclasses.h"
#include "hwallclasses.h"
#include "indexvariantmap.h"
#include "variant.h"
#include "statusmessagelist.h"
#include "audioallclasses.h"

/*
 * Metadata for PQs
 */
void BlackMisc::PhysicalQuantities::registerMetadata()
{
    CMeasurementUnit::registerMetadata();
    CAcceleration::registerMetadata();
    CAngle::registerMetadata();
    CFrequency::registerMetadata();
    CLength::registerMetadata();
    CMass::registerMetadata();
    CPressure::registerMetadata();
    CSpeed::registerMetadata();
    CTemperature::registerMetadata();
    CTime::registerMetadata();
    CPqString::registerMetadata();
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
    CCallsign::registerMetadata();
    CCallsignList::registerMetadata();
    CAtcStation::registerMetadata();
    CInformationMessage::registerMetadata();
    CAtcStationList::registerMetadata();
    CAircraft::registerMetadata();
    CAircraftList::registerMetadata();
    CAircraftSituation::registerMetadata();
    CAircraftIcao::registerMetadata();
    CAirportIcao::registerMetadata();
    CSelcal::registerMetadata();
    CFlightPlan::registerMetadata();
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
 * Metadata for Network
 */
void BlackMisc::Network::registerMetadata()
{
    CUser::registerMetadata();
    CUserList::registerMetadata();
    CServer::registerMetadata();
    CServerList::registerMetadata();
    CTextMessage::registerMetadata();
    CTextMessageList::registerMetadata();
    CClient::registerMetadata();
    CClientList::registerMetadata();
    CAircraftModel::registerMetadata();
    CVoiceCapabilities::registerMetadata();
    CAircraftMapping::registerMetadata();
    CAircraftMappingList::registerMetadata();
}

/*
 * Metadata for Settings
 */
void BlackMisc::Settings::registerMetadata()
{
    CSettingsNetwork::registerMetadata();
    CSettingsAudio::registerMetadata();
}

/*
 * Metadata for Voice
 */
void BlackMisc::Audio::registerMetadata()
{
    CAudioDevice::registerMetadata();
    CAudioDeviceList::registerMetadata();
    CVoiceRoom::registerMetadata();
    CVoiceRoomList::registerMetadata();
}

/*
 * Metadata for Hardware
 */
void BlackMisc::Hardware::registerMetadata()
{
    CKeyboardKey::registerMetadata();
    CKeyboardKeyList::registerMetadata();
}

/*
 * Metadata for Blackmisc
 */
void BlackMisc::registerMetadata()
{
    CVariant::registerMetadata();
    CIndexVariantMap::registerMetadata();

    CStatusMessage::registerMetadata();
    CStatusMessageList::registerMetadata();

    // sub namespaces
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
    Math::registerMetadata();
    Geo::registerMetadata();
    Network::registerMetadata();
    Settings::registerMetadata();
    Audio::registerMetadata();
    Hardware::registerMetadata();
}

/*
 * Init resources
 */
void BlackMisc::initResources()
{
    initBlackMiscResources();
}

/*
 * Stupid extension bo be able to compare 2 QVariants
 */
bool BlackMisc::equalQVariants(const QVariant &v1, const QVariant &v2)
{
    // prephase, shortcuts
    if (v1 == v2) return true; // compares on primitives or on address
    if (!v1.isValid() || !v2.isValid()) return false;
    if (v1.type() != v2.type()) return false;
    if (v1.userType() != v2.userType()) return false;

    // I have same types now
    const CValueObject *cs1 = CValueObject::fromQVariant(v1);
    const CValueObject *cs2 = CValueObject::fromQVariant(v2);
    if (cs1 && cs2)
    {
        int c = compare(*cs1, *cs2);
        return c == 0;
    }
    return  false;
}

/*
 * Compare values
 */
int BlackMisc:: compareQVariants(const QVariant &v1, const QVariant &v2)
{
    if (v1 == v2) return 0; // compares on primitives or on address

    if (!v1.isValid() || !v2.isValid()) qFatal("Invalid variants");
    if (v1.type() != v2.type()) qFatal("Mismatching types");
    if (v1.userType() != v2.userType()) qFatal("Mismatching user types");

    switch (v1.type())
    {
    case QMetaType::QString:
    case QMetaType::QChar:
        {
            QString s1 = v1.value<QString>();
            QString s2 = v2.value<QString>();
            return s1.compare(s2);
        }
    case QMetaType::QDateTime:
        {
            QDateTime dt1 = v1.value<QDateTime>();
            QDateTime dt2 = v2.value<QDateTime>();
            if (dt1 == dt2) return 0;
            return dt1 < dt2 ? -1 : 1;
        }
    case QMetaType::QDate:
        {
            QDate d1 = v1.value<QDate>();
            QDate d2 = v2.value<QDate>();
            if (d1 == d2) return 0;
            return d1 < d2 ? -1 : 1;
        }
    case QMetaType::QTime:
        {
            QTime t1 = v1.value<QTime>();
            QTime t2 = v2.value<QTime>();
            if (t1 == t2) return 0;
            return t1 < t2 ? -1 : 1;
        }
    case QMetaType::QPixmap:
        {
            QPixmap p1 = v1.value<QPixmap>();
            QPixmap p2 = v2.value<QPixmap>();
            if (p1.width() == p2.width()) return 0;
            return p1.width() < p2.width() ? -1 : 1;
        }
    default:
        break;
    }

    // BlackObject
    if (v1.type() == QVariant::UserType)
    {
        const CValueObject *cs1 = CValueObject::fromQVariant(v1);
        const CValueObject *cs2 = CValueObject::fromQVariant(v2);
        if (cs1 && cs2)
        {
            return compare(*cs1, *cs2);
        }
    }

    // all kind of numeric values
    if (v1.canConvert<double>())
    {
        double d1 = v1.value<double>();
        double d2 = v2.value<double>();
        if (d1 == d2) return 0;
        return d1 < d2 ? -1 : 1;
    }

    qFatal("Unknown type for compare");
    return -1;
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
 * Add hash values
 */
uint BlackMisc::calculateHash(const QList<uint> &values, const char *className)
{
    // http://stackoverflow.com/questions/113511/hash-code-implementation/113600#113600
    if (values.isEmpty()) return 0;
    uint hash = values.first();
    for (int i = 1; i < values.size(); i++)
    {
        hash = 37 * hash + values.at(i);
    }

    // same values, but different class?
    if (className)
    {
        hash = 37 * hash + qHash(QString(className));
    }
    return hash;
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
        {
            const char *name = QMetaType::typeName(type);
            qFatal("Type cannot be resolved: %s (%d)", name ? name : "", type);
        }
    }
    return QVariant(); // suppress compiler warning
}

#ifdef Q_CC_MSVC
#include <crtdbg.h>

/*
 * Heap size of an object
 */
size_t BlackMisc::heapSizeOf(const QMetaType &metaType)
{

    metaType.destroy(metaType.create()); // ignore one-off allocations of a class being instantiated for the first time
    _CrtMemState oldState, newState, diff;
    oldState.lTotalCount = newState.lTotalCount = diff.lTotalCount = 0; // avoid compiler warning
    diff.lSizes[_NORMAL_BLOCK] = 0;
    _CrtMemCheckpoint(&oldState);
    void *p = metaType.create();
    _CrtMemCheckpoint(&newState);
    metaType.destroy(p);
    _CrtMemDifference(&diff, &oldState, &newState);
    return diff.lSizes[_NORMAL_BLOCK];
}

/*
 * Heap size of an object
 */
size_t BlackMisc::heapSizeOf(const QMetaObject &metaObject)
{
    delete metaObject.newInstance(); //ignore one-off allocations of a class being instantiated for the first time
    _CrtMemState oldState, newState, diff;
    oldState.lTotalCount = newState.lTotalCount = diff.lTotalCount = 0; // avoid compiler warning
    diff.lSizes[_NORMAL_BLOCK] = 0;
    _CrtMemCheckpoint(&oldState);
    QObject *obj = metaObject.newInstance();
    _CrtMemCheckpoint(&newState);
    delete obj;
    _CrtMemDifference(&diff, &oldState, &newState);
    return diff.lSizes[_NORMAL_BLOCK];
}

#else //!Q_CC_MSVC

/*
 * Heap size of an object
 */
size_t BlackMisc::heapSizeOf(const QMetaType &)
{
    qDebug() << "heapSizeOf not supported by your compiler toolchain";
    return 0;
}
/*
 * Heap size of an object
 */
size_t BlackMisc::heapSizeOf(const QMetaObject &)
{
    qDebug() << "heapSizeOf not supported by your compiler toolchain";
    return 0;
}

#endif //!Q_CC_MSVC

/*
 * Dump all user types
 */
void BlackMisc::displayAllUserMetatypesTypes()
{
    for (int mt = QMetaType::User; mt < QMetaType::User + 1000; mt++)
    {
        if (!QMetaType::isRegistered(mt)) continue;
        QMetaType metaType(mt);
        qDebug() << "type:" << mt << "name:" << QMetaType::typeName(mt) << QMetaType::sizeOf(mt) << BlackMisc::heapSizeOf(metaType);
    }
}
