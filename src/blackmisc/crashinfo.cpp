/* Copyright (C) 2018
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
*/

#include "blackmisc/crashinfo.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/worker.h"

#include <QFile>
#include <QStringBuilder>

namespace BlackMisc
{
    CCrashInfo::CCrashInfo() {}

    QString CCrashInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("{ %1, %2 }").arg(this->getInfo(), this->getUserName());
    }

    void CCrashInfo::appendInfo(const QString &extraInfo)
    {
        if (extraInfo.isEmpty()) { return; }
        if (m_info.isEmpty()) { this->setInfo(extraInfo); return; }
        m_info += u' ' % extraInfo;
    }

    void CCrashInfo::setLogPathAndFileName(const QString &fileName)
    {
        m_logFileAndPath = fileName;
    }

    CVariant CCrashInfo::propertyByIndex(const CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUserName: return CVariant::fromValue(m_userName);
        case IndexInfo: return CVariant::fromValue(m_info);
        case IndexSimulatorString: return CVariant::fromValue(m_simulatorString);
        case IndexFlightNetworkInfo: return CVariant::fromValue(m_flightNetwork);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CCrashInfo::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.to<CCrashInfo>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUserName: this->setUserName(variant.toQString()); break;
        case IndexInfo: this->setUserName(variant.toQString()); break;
        case IndexSimulatorString: this->setSimulatorString(variant.toQString()); break;
        case IndexFlightNetworkInfo: this->setFlightNetworkString(variant.toQString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CCrashInfo::comparePropertyByIndex(const CPropertyIndex &index, const CCrashInfo &compareValue) const
    {
        if (index.isMyself()) { return this->getInfo().compare(compareValue.getInfo()); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUserName: return this->getUserName().compare(compareValue.getUserName());
        case IndexInfo: return this->getInfo().compare(compareValue.getInfo());
        case IndexSimulatorString: return this->getSimulatorString().compare(compareValue.getInfo());
        case IndexFlightNetworkInfo: return this->getFlightNetworkString().compare(compareValue.getFlightNetworkString());
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
} // ns
