// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/crashinfo.h"

#include "misc/fileutils.h"
#include "misc/worker.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CCrashInfo)

namespace swift::misc
{
    QString CCrashInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("{ %1, %2 }").arg(this->getInfo(), this->getUserName());
    }

    void CCrashInfo::appendInfo(const QString &extraInfo)
    {
        if (extraInfo.isEmpty()) { return; }
        if (m_info.isEmpty())
        {
            this->setInfo(extraInfo);
            return;
        }
        m_info += u' ' % extraInfo;
    }

    void CCrashInfo::setLogPathAndFileName(const QString &fileName) { m_logFileAndPath = fileName; }

    QVariant CCrashInfo::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        switch (index.frontCasted<ColumnIndex>())
        {
        case IndexUserName: return QVariant::fromValue(m_userName);
        case IndexInfo: return QVariant::fromValue(m_info);
        case IndexSimulatorString: return QVariant::fromValue(m_simulatorString);
        case IndexFlightNetworkInfo: return QVariant::fromValue(m_flightNetwork);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CCrashInfo::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CCrashInfo>();
            return;
        }
        switch (index.frontCasted<ColumnIndex>())
        {
        case IndexUserName: this->setUserName(variant.toString()); break;
        case IndexInfo: this->setInfo(variant.toString()); break;
        case IndexSimulatorString: this->setSimulatorString(variant.toString()); break;
        case IndexFlightNetworkInfo: this->setFlightNetworkString(variant.toString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CCrashInfo::comparePropertyByIndex(CPropertyIndexRef index, const CCrashInfo &compareValue) const
    {
        if (index.isMyself()) { return this->getInfo().compare(compareValue.getInfo()); }
        switch (index.frontCasted<ColumnIndex>())
        {
        case IndexUserName: return this->getUserName().compare(compareValue.getUserName());
        case IndexInfo: return this->getInfo().compare(compareValue.getInfo());
        case IndexSimulatorString: return this->getSimulatorString().compare(compareValue.getInfo());
        case IndexFlightNetworkInfo:
            return this->getFlightNetworkString().compare(compareValue.getFlightNetworkString());
        default: return CValueObject::comparePropertyByIndex(index.copyFrontRemoved(), compareValue);
        }
    }

    void CCrashInfo::triggerWritingFile() const
    {
        if (m_logFileAndPath.isEmpty()) { return; }
        CWorker::fromTask(qApp, Q_FUNC_INFO, [this] { writeToFile(); });
    }

    bool CCrashInfo::writeToFile() const
    {
        if (m_logFileAndPath.isEmpty()) { return false; }
        return CFileUtils::writeStringToFile(this->summary(), m_logFileAndPath);
    }

    QString CCrashInfo::summary() const
    {
        return (m_userName.isEmpty() ? QStringLiteral("") : u"user name: " % m_userName % u"\n") %
               (m_simulatorString.isEmpty() ? QStringLiteral("") : u"simulator: " % m_simulatorString % u"\n") %
               (m_flightNetwork.isEmpty() ? QStringLiteral("") : u"network: " % m_flightNetwork % u"\n") %
               (m_info.isEmpty() ? QStringLiteral("") : u"info: " % m_info % u"\n");
    }
} // namespace swift::misc
