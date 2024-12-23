// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUISTDAPPLICATION_H
#define SWIFTGUISTDAPPLICATION_H

#include <QCommandLineOption>
#include <QCoreApplication>
#include <QObject>
#include <QString>

#include "core/coremodeenums.h"
#include "gui/guiapplication.h"

/*!
 * Specialized GUI application for swift pilot client.
 * Handles parsing of some specialized CMD line argumenets and startup of core
 */
class CSwiftGuiStdApplication : public swift::gui::CGuiApplication
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
    virtual swift::misc::CStatusMessageList startHookIn() override;

private:
    static swift::core::CCoreFacadeConfig coreModeToCoreFacadeConfig(swift::core::CoreModes::CoreMode,
                                                                     const QString &dBusAddress);

    QCommandLineOption m_cmdFacadeMode { { "c", "core" },
                                         QCoreApplication::translate("main", "Core mode: (d)istributed, (s)tandalone."),
                                         "coremode" }; //!< Facade startup mode
};

#endif // SWIFTGUISTDAPPLICATION_H
