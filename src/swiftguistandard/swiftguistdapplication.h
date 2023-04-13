/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTGUISTDAPPLICATION_H
#define SWIFTGUISTDAPPLICATION_H

#include "blackgui/guiapplication.h"

#include <QCommandLineOption>
#include <QCoreApplication>
#include <QObject>
#include <QString>

/*!
 * Specialized GUI application for swift pilot client.
 * Handles parsing of some specialized CMD line argumenets and startup of core
 */
class CSwiftGuiStdApplication : public BlackGui::CGuiApplication
{
    Q_OBJECT

public:
    //! Constructor
    CSwiftGuiStdApplication();

    //! Single instance
    CSwiftGuiStdApplication *instance();

protected:
    //! Parsing of special CMD args
    virtual bool parsingHookIn() override;

    //! Start facade by cmd arguments
    virtual BlackMisc::CStatusMessageList startHookIn() override;

private:
    QCommandLineOption m_cmdFacadeMode {
        { "c", "core" },
        QCoreApplication::translate("main", "Core mode: (d)istributed, (s)tandalone."),
        "coremode"
    }; //!< Facade startup mode
};

#endif // guard
