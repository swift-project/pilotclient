/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/identifier.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"

#include <QCoreApplication>
#include <QHostInfo>
#include <QSysInfo>
#include <QStringBuilder>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#ifdef Q_OS_MAC
#include <sys/sysctl.h>
#endif

//! \private Remove characters not allowed in dbus paths
QString sanitizeForDBusPath(const QString &s)
{
    return BlackMisc::removeChars(s, [](QChar c) { return !c.isUpper() && !c.isLower() && !c.isDigit() && c != '_' && c != '/'; });
}

//! \private Remove characters not allowed in dbus path elements
QString sanitizeForDBusPathElement(const QString &s)
{
    return BlackMisc::removeChars(s, [](QChar c) { return !c.isUpper() && !c.isLower() && !c.isDigit() && c != '_'; });
}

//! \private
const QString &cachedSanitizedApplicationName()
{
    static const QString appName = sanitizeForDBusPathElement(QCoreApplication::applicationName());
    return appName;
}

//! \private
const QString &cachedLocalHostName()
{
    static const QString hostName = QHostInfo::localHostName();
    return hostName;
}

enum
{
    UuidStringLen = sizeof("00000000-0000-0000-0000-000000000000")
};

QByteArray getMachineUniqueIdImpl()
{
    // TODO RR: Remove the workaround branches as soon as the following two changes are published in 5.12.2 (TBC)
    // https://codereview.qt-project.org/#/c/249256
    // https://codereview.qt-project.org/#/c/249399

    QByteArray machineUniqueId;
#ifdef Q_OS_MAC
    char uuid[UuidStringLen];
    size_t uuidlen = sizeof(uuid);
    int ret = sysctlbyname("kern.uuid", uuid, &uuidlen, nullptr, 0);
    if (ret == 0 && uuidlen == sizeof(uuid))
    {
        machineUniqueId = QByteArray(uuid, uuidlen - 1);
    }
#elif defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
    HKEY key = nullptr;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS)
    {
        wchar_t buffer[UuidStringLen];
        DWORD size = sizeof(buffer);
        bool ok = (RegQueryValueEx(key, L"MachineGuid", nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &size) == ERROR_SUCCESS);
        RegCloseKey(key);
        if (ok) { machineUniqueId = QStringView(buffer, (size - 1) / 2).toLatin1(); }
    }
#else
    machineUniqueId = QSysInfo::machineUniqueId();
#endif
    return machineUniqueId;
}

QByteArray cachedMachineUniqueId()
{
    static const QByteArray machineUniqueId = getMachineUniqueIdImpl();
    return machineUniqueId;
}

namespace BlackMisc
{
    CIdentifier::CIdentifier(const QString &name)
        : ITimestampBased(QDateTime::currentMSecsSinceEpoch()),
          m_name(name.trimmed()),
          m_machineIdBase64(cachedMachineUniqueId().toBase64()),
          m_machineName(cachedLocalHostName()),
          m_processName(cachedSanitizedApplicationName()),
          m_processId(QCoreApplication::applicationPid())
    { }

    CIdentifier::CIdentifier(const QString &name, QObject *object) : CIdentifier(name)
    {
        if (object)
        {
            // append object name
            this->linkWithQObjectName(object);
            this->appendName(object->objectName());
        }
    }

    CIdentifier::CIdentifier(const QString &name, const QString &machineId, const QString &machineName,
                             const QString &processName, qint64 processId) :
        ITimestampBased(QDateTime::currentMSecsSinceEpoch()),
        m_name(name), m_machineIdBase64(machineId), m_machineName(machineName),
        m_processName(processName), m_processId(processId)
    { }

    const CIdentifier &CIdentifier::anonymous()
    {
        static const CIdentifier id("");
        return id;
    }

    const CIdentifier &CIdentifier::null()
    {
        static const CIdentifier id("", "", "", "", 0);
        return id;
    }

    const CIdentifier &CIdentifier::fake()
    {
        static const CIdentifier id("fake", QByteArrayLiteral("00000000-0000-0000-0000-000000000000").toBase64(), "fake machine", "fake process", 0);
        return id;
    }

    QUuid CIdentifier::toUuid() const
    {
        static const QUuid ns = QUuid::createUuid();
        QByteArray baseData;
        baseData.append(getMachineId());
        baseData.append(reinterpret_cast<const char *>(&m_processId), sizeof(m_processId));
        baseData.append(reinterpret_cast<const char *>(&m_timestampMSecsSinceEpoch), sizeof(m_timestampMSecsSinceEpoch));
        baseData.append(getName());
        return QUuid::createUuidV5(ns, baseData);
    }

    QString CIdentifier::toUuidString() const
    {
        return toUuid().toString();
    }

    void CIdentifier::appendName(const QString &name)
    {
        if (m_name.endsWith(name)) { return; }
        if (name.isEmpty()) { return; }
        const int index = m_name.lastIndexOf(':');
        if (index >= 0) { m_name = m_name.left(index); }
        m_name += QStringLiteral(":") + name;
    }

    void CIdentifier::linkWithQObjectName(QObject *object)
    {
        if (!object) { return; }
        QObject::connect(object, &QObject::objectNameChanged, object, [ = ](const QString & name) { this->appendName(name); });
    }

    QByteArray CIdentifier::getMachineId() const
    {
        return QByteArray::fromBase64(m_machineIdBase64.toLocal8Bit());
    }

    QString CIdentifier::toDBusObjectPath(const QString &root) const
    {
        QString path = root;
        path += '/' % sanitizeForDBusPathElement(m_machineName) % '_' % m_machineIdBase64;
        path += '/' % sanitizeForDBusPathElement(m_processName) % '_' % QString::number(m_processId);

        const QString name = sanitizeForDBusPath(m_name);
        Q_ASSERT_X(!name.contains("//") && !name.startsWith('/') && !name.endsWith('/'), Q_FUNC_INFO, "Invalid name");
        if (!name.isEmpty()) { path += '/' % name; }
        return path;
    }

    CIdentifier CIdentifier::fromDBusObjectPath(const QString &path, const QString &root)
    {
        const QString relative = path.startsWith(root) ? path.mid(root.length()) : path;
        const QString machine = relative.section('/', 0, 0);
        const QString process = relative.section('/', 1, 1);
        const QString name = relative.section('/', 2, -1);

        CIdentifier result(name);
        result.m_machineIdBase64 = machine.section('_', -1, -1);
        result.m_machineName = machine.section('_', 0, -2);
        result.m_processId = process.section('_', -1, -1).toInt();
        result.m_processName = process.section('_', 0, -2);
        return result;
    }

    bool CIdentifier::hasSameMachineName(const CIdentifier &other) const
    {
        return !other.getMachineName().isEmpty() && other.getMachineName() == this->getMachineName();
    }

    bool CIdentifier::hasSameMachineId(const CIdentifier &other) const
    {
        return !m_machineIdBase64.isEmpty() && m_machineIdBase64 == other.m_machineIdBase64;
    }

    bool CIdentifier::hasSameMachineNameOrId(const CIdentifier &other) const
    {
        return this->hasSameMachineId(other) || this->hasSameMachineName(other);
    }

    bool CIdentifier::isFromLocalMachine() const
    {
        //! \fixme KB 2019-02 wonder if we should check on id (strict) or machine name (lenient)
        return cachedMachineUniqueId() == getMachineId();
    }

    bool CIdentifier::hasApplicationProcessId() const
    {
        return QCoreApplication::applicationPid() == getProcessId() && isFromLocalMachine();
    }

    bool CIdentifier::hasApplicationProcessName() const
    {
        return cachedSanitizedApplicationName() == sanitizeForDBusPathElement(getProcessName());
    }

    bool CIdentifier::isAnonymous() const
    {
        return &anonymous() == this || anonymous() == *this;
    }

    bool CIdentifier::isNull() const
    {
        return &null() == this || null() == *this;
    }

    void CIdentifier::updateToCurrentMachine()
    {
        m_machineIdBase64 = cachedMachineUniqueId().toBase64();
        m_machineName     = cachedLocalHostName();
    }

    void CIdentifier::updateToCurrentProcess()
    {
        m_processName = QCoreApplication::applicationName();
        m_processId   = QCoreApplication::applicationPid();
    }

    QString CIdentifier::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        const QString s = m_name %
                          u' ' % m_machineIdBase64 %
                          u' ' % m_machineName %
                          u' ' % QString::number(m_processId) %
                          u' ' % m_processName;
        return s;
    }

    CVariant CIdentifier::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        switch (i)
        {
        case IndexName:               return CVariant::fromValue(m_name);
        case IndexMachineIdBase64:    return CVariant::fromValue(m_machineIdBase64);
        case IndexMachineName:        return CVariant::fromValue(getMachineName());
        case IndexMachineId:          return CVariant::fromValue(getMachineId());
        case IndexProcessId:          return CVariant::fromValue(m_processId);
        case IndexProcessName:        return CVariant::fromValue(m_processName);
        case IndexIsFromLocalMachine: return CVariant::fromValue(isFromLocalMachine());
        case IndexIsFromSameProcess:  return CVariant::fromValue(hasApplicationProcessId());
        case IndexIsFromSameProcessName: return CVariant::fromValue(hasApplicationProcessName());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    int CIdentifier::comparePropertyByIndex(const CPropertyIndex &index, const CIdentifier &compareValue) const
    {
        if (index.isMyself()) { return Compare::compare(m_processId, compareValue.m_processId); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }

        switch (i)
        {
        case IndexName:               return m_name.compare(compareValue.m_name, Qt::CaseInsensitive);
        case IndexMachineIdBase64:    return m_machineIdBase64.compare(compareValue.m_machineIdBase64, Qt::CaseInsensitive);
        case IndexMachineName:        return m_machineName.compare(compareValue.m_machineName, Qt::CaseInsensitive);
        case IndexMachineId:          return m_machineName.compare(compareValue.m_machineName, Qt::CaseInsensitive);
        case IndexProcessId:          return Compare::compare(m_processId, compareValue.m_processId);
        case IndexProcessName:        return m_processName.compare(compareValue.m_processName, Qt::CaseInsensitive);
        case IndexIsFromLocalMachine: return Compare::compare(this->isFromLocalMachine(), compareValue.isFromLocalMachine());
        case IndexIsFromSameProcess:  return Compare::compare(this->hasApplicationProcessId(), compareValue.hasApplicationProcessId());
        case IndexIsFromSameProcessName: return Compare::compare(this->hasApplicationProcessName(), compareValue.hasApplicationProcessName());
        default: return CValueObject::comparePropertyByIndex(index, compareValue);
        }
    }

    void CIdentifier::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        CValueObject::setPropertyByIndex(index, variant);
    }

} // ns
