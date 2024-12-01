// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PROCESS_H
#define SWIFT_MISC_PROCESS_H

#include <QProcess>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Wrapper around QProcess with additional features
     */
    class SWIFT_MISC_EXPORT CProcessCtrl : public QProcess
    {
    public:
        Q_OBJECT

    public:
        //! Constructor.
        CProcessCtrl(QObject *parent = nullptr);

        //! Start a programm detached and without any console window
        static bool startDetached(const QString &program, const QStringList &arguments, bool withConsoleWindow);
    };
} // namespace swift::misc

#endif // SWIFT_MISC_PROCESS_H
