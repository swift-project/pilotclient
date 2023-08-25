// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PROCESSINFO_H
#define BLACKMISC_PROCESSINFO_H

#include "blackmisc/valueobject.h"
#include <QCoreApplication>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc, CProcessInfo)

namespace BlackMisc
{
    /*!
     * Value class identifying a process, with a PID and a name.
     */
    class BLACKMISC_EXPORT CProcessInfo : public CValueObject<CProcessInfo>
    {
    public:
        //! Construct an object identifying an invalid process.
        CProcessInfo() {}

        //! Construct an object identifying the process with the given PID.
        explicit CProcessInfo(qint64 pid) : m_pid(pid), m_name(processNameFromId(pid)) {}

        //! Construct an object identifying a process with the given PID and name.
        CProcessInfo(qint64 pid, const QString &name) : m_pid(pid), m_name(name) {}

        //! Return an object identifying the current process.
        static CProcessInfo currentProcess() { return CProcessInfo(QCoreApplication::applicationPid()); }

        //! True if this object identifies a process that exists.
        bool exists() const { return !m_name.isEmpty() && *this == CProcessInfo(m_pid); }

        //! Get the pid.
        qint64 processId() const { return m_pid; }

        //! Empty process info
        bool isNull() const { return m_pid == 0 && m_name.isEmpty(); }

        //! Get the process name.
        const QString &processName() const { return m_name; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        static QString processNameFromId(qint64 pid);

        qint64 m_pid = 0;
        QString m_name;

        BLACK_METACLASS(
            CProcessInfo,
            BLACK_METAMEMBER(pid),
            BLACK_METAMEMBER(name)
        );
    };
}

Q_DECLARE_METATYPE(BlackMisc::CProcessInfo)

#endif
