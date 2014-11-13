/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmiscfreefunctions.h"
#include "avallclasses.h"
#include "pqallquantities.h"
#include "mathallclasses.h"
#include "geoallclasses.h"
#include "networkallclasses.h"
#include "audioallclasses.h"
#include "hwallclasses.h"
#include "settingsblackmiscclasses.h"
#include "propertyindexlist.h"
#include "propertyindexvariantmap.h"
#include "namevariantpairlist.h"
#include "variant.h"
#include "statusmessagelist.h"
#include "iconlist.h"
#include "eventallclasses.h"
#include <QtNetwork/QHostInfo>
#include <QProcessEnvironment>
#include <QSysInfo>
#include <QProcess>

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
 * Metadata for Settings
 */
void BlackMisc::Settings::registerMetadata()
{
    CSettingsNetwork::registerMetadata();
    CSettingsAudio::registerMetadata();
    CSettingKeyboardHotkey::registerMetadata();
    CSettingKeyboardHotkeyList::registerMetadata();
}

/*
 * Metadata for Audio
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
    CJoystickButton::registerMetadata();
}

/*
 * Metadata for Event
 */
void BlackMisc::Event::registerMetadata()
{
    COriginator::registerMetadata();
    CEventHotkeyFunction::registerMetadata();
}

/*
 * Metadata for Blackmisc
 */
void BlackMisc::registerMetadata()
{
    CPropertyIndex::registerMetadata();
    CVariant::registerMetadata();
    CPropertyIndex::registerMetadata();
    CPropertyIndexList::registerMetadata();
    CPropertyIndexVariantMap::registerMetadata();
    CNameVariantPair::registerMetadata();
    CNameVariantPairList::registerMetadata();
    CStatusMessage::registerMetadata();
    CStatusMessageList::registerMetadata();
    CIcon::registerMetadata();
    CIconList::registerMetadata();
    CHotkeyFunction::registerMetadata();
    CLogCategory::registerMetadata();
    CLogCategoryList::registerMetadata();

    // sub namespaces
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
    Math::registerMetadata();
    Geo::registerMetadata();
    Network::registerMetadata();
    Settings::registerMetadata();
    Audio::registerMetadata();
    Hardware::registerMetadata();
    Event::registerMetadata();

    // needed by XBus proxy class
    qRegisterMetaType<CSequence<double>>();
    qRegisterMetaType<CSequence<double>>("CDoubleSequence");
    qDBusRegisterMetaType<CSequence<double>>();
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
    // Compares this QVariant with v and returns true if they are equal; otherwise returns false.
    // In the case of custom types, their equalness operators are not called. Instead the values' addresses are compared.
    if (v1 == v2) return true;

    // shortcuts
    if (!v1.isValid() || !v2.isValid()) return false;
    if (v1.type() != v2.type()) return false;
    if (v1.userType() != v2.userType()) return false;

    // I have same types now
    int c = compareQVariants(v1, v2);
    return  c == 0;
}

/*
 * Compare values
 */
int BlackMisc::compareQVariants(const QVariant &v1, const QVariant &v2)
{
    // Compares this QVariant with v and returns true if they are equal; otherwise returns false.
    // In the case of custom types, their equalness operators are not called. Instead the values' addresses are compared.
    if (v1 == v2) return 0;

    if (!v1.isValid() || !v2.isValid()) qFatal("Invalid variants");
    if (v1.type() != v2.type()) qFatal("Mismatching types");
    if (v1.userType() != v2.userType()) qFatal("Mismatching user types");

    auto v1Type = static_cast<QMetaType::Type>(v1.type());
    switch (v1Type)
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

    // CValueObject
    if (v1Type == QMetaType::User)
    {
        const CValueObject *cs1 = CValueObject::fromQVariant(v1);
        const CValueObject *cs2 = CValueObject::fromQVariant(v2);
        if (cs1 && cs2)
        {
            return compare(*cs1, *cs2);
        }
    }

    // integer types, handling not as double for rounding issues
    if (v1Type == QMetaType::Int)
    {
        int i1 = v1.value<int>();
        int i2 = v2.value<int>();
        if (i1 == i2) return 0;
        return i1 < i2 ? -1 : 1;
    }
    else if (v1Type == QMetaType::UInt)
    {
        uint i1 = v1.value<uint>();
        uint i2 = v2.value<uint>();
        if (i1 == i2) return 0;
        return i1 < i2 ? -1 : 1;
    }
    else if (v1Type == QMetaType::LongLong)
    {
        long long i1 = v1.value<long long>();
        long long i2 = v2.value<long long>();
        if (i1 == i2) return 0;
        return i1 < i2 ? -1 : 1;
    }
    else if (v1Type == QMetaType::ULongLong)
    {
        unsigned long long i1 = v1.value<unsigned long long>();
        unsigned long long i2 = v2.value<unsigned long long>();
        if (i1 == i2) return 0;
        return i1 < i2 ? -1 : 1;
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
 * Add hash values
 */
uint BlackMisc::calculateHash(const QList<int> &values, const char *className)
{
    QList<uint> list;
    uint s = 0;
    foreach(int i, values)
    {

        if (i >= 0)
        {
            list.append(static_cast<uint>(i));
        }
        else
        {
            list.append(static_cast<uint>(i));
            list.append(s++);
        }
    }
    return calculateHash(list, className);
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
        if (localUserType < static_cast<int>(QVariant::UserType))
        {
            // complex Qt type, e.g. QDateTime
            return BlackMisc::complexQtTypeFromDbusArgument(arg, localUserType);
        }
        else
        {
            QVariant valueVariant(localUserType, nullptr);
            auto *meta = Private::getValueObjectMetaInfo(valueVariant);
            if (meta)
            {
                meta->unmarshall(arg, valueVariant.data());
                return valueVariant;
            }
        }
    }
    else
    {
        qWarning() << "fixQVariantFromDbusArgument called with unsupported type";
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

/*
 * Local host name
 */
const QString &BlackMisc::localHostName()
{
    static const QString hostName = QHostInfo::localHostName();
    return hostName;
}

/*
 * Local host name
 */
const QString &BlackMisc::localHostNameEnvVariable()
{
    static const QString hostName = QProcessEnvironment::systemEnvironment().value("COMPUTERNAME", QProcessEnvironment::systemEnvironment().value("HOSTNAME"));
    return hostName;
}

/*
 * Window mixer
 */
bool BlackMisc::Audio::startWindowsMixer()
{
    QStringList parameterlist;
    return QProcess::startDetached("SndVol.exe", parameterlist);
}
