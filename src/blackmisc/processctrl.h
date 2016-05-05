/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROCESS_H
#define BLACKMISC_PROCESS_H

#include "blackmiscexport.h"
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
        static bool startDetachedWithoutConsole(const QString &program, const QStringList &arguments);
    };
} // ns

#endif // guard
