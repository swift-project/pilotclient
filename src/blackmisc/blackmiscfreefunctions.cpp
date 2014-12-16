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
    CAudioDeviceInfo::registerMetadata();
    CAudioDeviceInfoList::registerMetadata();
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
    qWarning() << "fixQVariantFromDbusArgument called with unsupported type";
    return variant;
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

template<class K, class V> QString BlackMisc::qmapToString(const QMap<K, V> &map)
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

// forward declare: http://www.parashift.com/c++-faq-lite/separate-template-fn-defn-from-decl.html
template QString BlackMisc::qmapToString<QString, int>(const QMap<QString, int> &);
template QString BlackMisc::qmapToString<QString, QString>(const QMap<QString, QString> &);
template QString BlackMisc::qmapToString<QString, double>(const QMap<QString, double> &);

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
