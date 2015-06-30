/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmiscfreefunctions.h"
#include "math/math.h"
#include "geo/geo.h"
#include "audio/audio.h"
#include "hardware/hardware.h"
#include "settingsblackmiscclasses.h"
#include "propertyindexlist.h"
#include "propertyindexvariantmap.h"
#include "namevariantpairlist.h"
#include "variantlist.h"
#include "variantmap.h"
#include "statusmessagelist.h"
#include "pixmap.h"
#include "iconlist.h"
#include "identifier.h"
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
}

/*
 * Metadata for Geo
 */
void BlackMisc::Geo::registerMetadata()
{
    CCoordinateGeodetic::registerMetadata();
    CLatitude::registerMetadata();
    CLongitude::registerMetadata();
}

/*
 * Metadata for Settings
 */
void BlackMisc::Settings::registerMetadata()
{
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
    CEventHotkeyFunction::registerMetadata();
}

/*
 * Metadata for Blackmisc
 */
void BlackMisc::registerMetadata()
{
    CPropertyIndex::registerMetadata();
    CVariant::registerMetadata();
    CVariantList::registerMetadata();
    CVariantMap::registerMetadata();
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
    CPixmap::registerMetadata();
    CIdentifier::registerMetadata();

    // sub namespaces
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
    Math::registerMetadata();
    Geo::registerMetadata();
    Network::registerMetadata();
    Settings::registerMetadata();
    Simulation::registerMetadata();
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
    // my business?
    if (!variant.canConvert<QDBusArgument>()) { return variant; }

    // complex, user type
    // it has to be made sure, that the cast works
    const QDBusArgument arg = variant.value<QDBusArgument>();
    const int userType = static_cast<int>(QVariant::UserType);
    if (localUserType < userType)
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
        Q_ASSERT_X(false, Q_FUNC_INFO, "no meta");
        return valueVariant;
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
void BlackMisc::displayAllUserMetatypesTypes(QTextStream &out)
{

    for (int mt = QMetaType::User; mt < QMetaType::User + 1000; mt++)
    {
        if (!QMetaType::isRegistered(mt)) { continue; }
        QMetaType metaType(mt);
        out << "type: " << mt << " name:" << QMetaType::typeName(mt) << " | "
            << QMetaType::sizeOf(mt) << " / " << BlackMisc::heapSizeOf(metaType) << endl;
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

QJsonObject BlackMisc::getIncrementalObject(const QJsonObject &previousObject, const QJsonObject &currentObject)
{
    QJsonObject incrementalObject = currentObject;
    for (const auto &key : previousObject.keys())
    {
        if (previousObject.value(key).isObject())
        {
            auto child = getIncrementalObject(previousObject.value(key).toObject(), currentObject.value(key).toObject());
            if (child.isEmpty()) incrementalObject.remove(key);
            else incrementalObject.insert(key, child);
        }
        else
        {
            if (currentObject.value(key) == previousObject.value(key))
                incrementalObject.remove(key);
        }
    }
    return incrementalObject;
}

QJsonObject BlackMisc::applyIncrementalObject(const QJsonObject &previousObject, const QJsonObject &incrementalObject)
{
    QJsonObject currentObject = previousObject;
    for (const auto &key : incrementalObject.keys())
    {
        // If it is not an object, just insert the value
        if (!incrementalObject.value(key).isObject())
        {
            currentObject.insert(key, incrementalObject.value(key));
        }
        else
        {
            auto child = applyIncrementalObject(currentObject.value(key).toObject(), incrementalObject.value(key).toObject());
            currentObject.insert(key, child);
        }
    }
    return currentObject;
}


QString BlackMisc::boolToOnOff(bool v, bool i18n)
{
    Q_UNUSED(i18n);
    return v ? "on" : "off";
}

QString BlackMisc::boolToYesNo(bool v, bool i18n)
{
    Q_UNUSED(i18n);
    return v ? "yes" : "no";
}

QString BlackMisc::boolToTrueFalse(bool v, bool i18n)
{
    Q_UNUSED(i18n);
    return v ? "true" : "false";
}
