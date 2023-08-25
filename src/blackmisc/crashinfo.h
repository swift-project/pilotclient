// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_CRASHINFO_H
#define BLACKMISC_CRASHINFO_H

#include "blackmisc/valueobject.h"
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc, CCrashInfo)

namespace BlackMisc
{
    /*!
     * Crash info. Details about crash context.
     */
    class BLACKMISC_EXPORT CCrashInfo : public CValueObject<CCrashInfo>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexUserName = CPropertyIndexRef::GlobalIndexCCrashInfo,
            IndexInfo,
            IndexSimulatorString,
            IndexFlightNetworkInfo
        };

        //! Default constructor.
        CCrashInfo();

        //! Get user name
        const QString &getUserName() const { return m_userName; }

        //! User name
        void setUserName(const QString &userName) { m_userName = userName; }

        //! Any info available?
        bool hasInfo() const { return !m_info.isEmpty(); }

        //! Get the info
        const QString &getInfo() const { return m_info; }

        //! Set info
        void setInfo(const QString &info) { m_info = info; }

        //! Simulator string
        const QString &getSimulatorString() const { return m_simulatorString; }

        //! Simulator string
        void setSimulatorString(const QString &simString) { m_simulatorString = simString; }

        //! Network string
        const QString &getFlightNetworkString() const { return m_flightNetwork; }

        //! Network string
        void setFlightNetworkString(const QString &network) { m_flightNetwork = network; }

        //! Append some info
        void appendInfo(const QString &extraInfo);

        //! Set path and file name
        void setLogPathAndFileName(const QString &fileName);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CCrashInfo &compareValue) const;

        //! Trigger writing this to file (in background)
        void triggerWritingFile() const;

        //! Write to file (synchronous)
        bool writeToFile() const;

        //! Summary
        QString summary() const;

    private:
        QString m_userName;
        QString m_info;
        QString m_simulatorString;
        QString m_flightNetwork;
        QString m_logFileAndPath;

        BLACK_METACLASS(
            CCrashInfo,
            BLACK_METAMEMBER(userName),
            BLACK_METAMEMBER(info),
            BLACK_METAMEMBER(simulatorString),
            BLACK_METAMEMBER(flightNetwork),
            BLACK_METAMEMBER(logFileAndPath)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CCrashInfo)

#endif // guard
