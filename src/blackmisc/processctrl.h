// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PROCESS_H
#define BLACKMISC_PROCESS_H

#include "blackmisc/blackmiscexport.h"
#include <QProcess>

namespace BlackMisc
{
    /*!
     * Wrapper around QProcess with additional features
     */
    class BLACKMISC_EXPORT CProcessCtrl : public QProcess
    {
    public:
        Q_OBJECT

    public:
        //! Constructor.
        CProcessCtrl(QObject *parent = nullptr);

        //! Start a programm detached and without any console window
        static bool startDetached(const QString &program, const QStringList &arguments, bool withConsoleWindow);
    };
} // ns

#endif // guard
