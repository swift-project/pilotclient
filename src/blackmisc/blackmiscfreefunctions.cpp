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
#include "input/input.h"
#include "settingsblackmiscclasses.h"
#include "propertyindexlist.h"
#include "propertyindexvariantmap.h"
#include "namevariantpairlist.h"
#include "variantlist.h"
#include "variantmap.h"
#include "rgbcolor.h"
#include "countrylist.h"
#include "statusmessagelist.h"
#include "pixmap.h"
#include "iconlist.h"
#include "identifierlist.h"
#include "logpattern.h"
#include <QtNetwork/QHostInfo>
#include <QProcessEnvironment>
#include <QSysInfo>
#include <QProcess>
#include <QBuffer>

void BlackMisc::Math::registerMetadata()
{
    // void
}

void BlackMisc::Geo::registerMetadata()
{
    CCoordinateGeodetic::registerMetadata();
    CLatitude::registerMetadata();
    CLongitude::registerMetadata();
}

void BlackMisc::Settings::registerMetadata()
{
    CSettingsAudio::registerMetadata();
}

void BlackMisc::Audio::registerMetadata()
{
    CAudioDeviceInfo::registerMetadata();
    CAudioDeviceInfoList::registerMetadata();
    CVoiceRoom::registerMetadata();
    CVoiceRoomList::registerMetadata();
    qDBusRegisterMetaType<BlackSound::CNotificationSounds::PlayMode>();
    qDBusRegisterMetaType<BlackSound::CNotificationSounds::Notification>();
}

void BlackMisc::Input::registerMetadata()
{
    CKeyboardKey::registerMetadata();
    CKeyboardKeyList::registerMetadata();
    CJoystickButton::registerMetadata();
    CJoystickButtonList::registerMetadata();
    CActionHotkey::registerMetadata();
    CActionHotkeyList::registerMetadata();
    CHotkeyCombination::registerMetadata();
    qDBusRegisterMetaType<KeyCode>();
}

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
    CLogCategory::registerMetadata();
    CLogCategoryList::registerMetadata();
    CLogPattern::registerMetadata();
    CPixmap::registerMetadata();
    CIdentifier::registerMetadata();
    CIdentifierList::registerMetadata();
    CRgbColor::registerMetadata();
    CCountry::registerMetadata();
    CCountryList::registerMetadata();

    // sub namespaces
    PhysicalQuantities::registerMetadata();
    Aviation::registerMetadata();
    Math::registerMetadata();
    Geo::registerMetadata();
    Network::registerMetadata();
    Settings::registerMetadata();
    Simulation::registerMetadata();
    Audio::registerMetadata();
    Weather::registerMetadata();
    Input::registerMetadata();

    // needed by XBus proxy class
    qRegisterMetaType<CSequence<double>>();
    qRegisterMetaType<CSequence<double>>("CDoubleSequence");
    qDBusRegisterMetaType<CSequence<double>>();
}

void BlackMisc::initResources()
{
    initBlackMiscResources();
}

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

QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type);

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
        return complexQtTypeFromDbusArgument(arg, localUserType);
    }
    else if (QMetaType(localUserType).flags() & QMetaType::IsEnumeration)
    {
        arg.beginStructure();
        int i;
        arg >> i;
        arg.endStructure();

        QVariant valueVariant = QVariant::fromValue(i);
        bool ok = valueVariant.convert(localUserType);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "int could not be converted to enum");
        Q_UNUSED(ok);
        return valueVariant;
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

QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type)
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

size_t BlackMisc::heapSizeOf(const QMetaType &)
{
    // qDebug() << "heapSizeOf not supported by your compiler toolchain";
    return 0;
}

size_t BlackMisc::heapSizeOf(const QMetaObject &)
{
    // qDebug() << "heapSizeOf not supported by your compiler toolchain";
    return 0;
}

#endif //!Q_CC_MSVC

void BlackMisc::displayAllUserMetatypesTypes(QTextStream &out)
{
    out << getAllUserMetatypesTypes();
}

QString BlackMisc::getAllUserMetatypesTypes(const QString &separator)
{
    int fails = 0;
    QString meta;
    for (int mt = QMetaType::User; mt < QMetaType::User + 1000; mt++)
    {
        if (!QMetaType::isRegistered(mt))
        {
            fails++;
            // normally a consecutive order of metatypes, we allow a space before we break
            if (fails > 3) { return meta; }
            continue;
        }
        QMetaType metaType(mt);
        meta = meta.
               append("type: ").append(QString::number(mt)).
               append(" name: ").append(QMetaType::typeName(mt)).
               append(" | ").append(QString::number(QMetaType::sizeOf(mt))).
               append(" / ").append(QString::number(BlackMisc::heapSizeOf(metaType))).
               append(separator);
    }
    return meta;
}

const QString &BlackMisc::localHostName()
{
    static const QString hostName = QHostInfo::localHostName();
    return hostName;
}

const QString &BlackMisc::localHostNameEnvVariable()
{
    static const QString hostName = QProcessEnvironment::systemEnvironment().value("COMPUTERNAME", QProcessEnvironment::systemEnvironment().value("HOSTNAME"));
    return hostName;
}

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

bool BlackMisc::stringToBool(const QString &string)
{
    QString s(string.trimmed().toLower());
    if (s.isEmpty()) { return false; }
    QChar c = s.at(0);

    // explicit values
    if (c == '1' || c == 't' || c == 'y' || c == 'x') { return true; }
    if (c == '0' || c == 'f' || c == 'n' || c == '_') { return false; }
    return false;
}

QString BlackMisc::intToHex(int value, int digits)
{
    QString hex(QString::number(value, 16).toUpper());
    int l = hex.length();
    if (l >= digits) { return hex.right(digits); }
    int d = digits - l;
    return QString(d, '0') + hex;
}

bool BlackMisc::pixmapToPngByteArray(const QPixmap &pixmap, QByteArray &array)
{
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    bool s = pixmap.save(&buffer, "PNG");
    buffer.close();
    return s;
}

QString BlackMisc::bytesToHexString(const QByteArray &bytes)
{
    QString h;
    for (int i = 0; i < bytes.size(); i++)
    {
        int b = static_cast<int>(bytes.at(i));
        h.append(intToHex(b, 2));
    }
    return h;
}

QByteArray BlackMisc::byteArrayFromHexString(const QString &hexString)
{
    QByteArray ba;
    int pos = 0;
    while (pos + 1 < hexString.length())
    {
        bool ok;
        QString h = hexString.mid(pos, 2);
        int hex = h.toInt(&ok, 16);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "Invalid hex");
        if (!ok) { return QByteArray(); }
        ba.push_back(static_cast<char>(hex));
        pos += 2;
    }
    return ba;
}

QPixmap BlackMisc::pngByteArrayToPixmap(const QByteArray &array)
{
    if (array.isEmpty()) { return QPixmap(); }
    QPixmap p;
    bool s = p.loadFromData(array, "PNG");
    return s ? p : QPixmap();
}

bool BlackMisc::pngByteArrayToPixmapRef(const QByteArray &array, QPixmap &pixmap)
{
    if (array.isEmpty()) { return false; }
    bool s = pixmap.loadFromData(array, "PNG");
    return s;
}

QString BlackMisc::pixmapToPngHexString(const QPixmap &pixmap)
{
    QByteArray ba;
    bool s = pixmapToPngByteArray(pixmap, ba);
    if (!s) { return QString(); }
    return bytesToHexString(ba);
}

QPixmap BlackMisc::pngHexStringToPixmap(const QString &hexString)
{
    if (hexString.isEmpty()) { return QPixmap(); }
    QByteArray ba(byteArrayFromHexString(hexString));
    return pngByteArrayToPixmap(ba);
}

bool BlackMisc::pngHexStringToPixmapRef(const QString &hexString, QPixmap &pixmap)
{
    if (hexString.isEmpty()) { return false; }
    QByteArray ba(byteArrayFromHexString(hexString));
    return pngByteArrayToPixmapRef(ba, pixmap);
}
