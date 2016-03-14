/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTGUISTDAPPLICATION_H
#define SWIFTGUISTDAPPLICATION_H

#include "blackgui/guiapplication.h"
#include "blackcore/corefacadeconfig.h"

/*!
 * Specialized GUI application for swift pilot client.
 * Handles parsing of some specialized CMD line argumenets and startup of core
 */
class CSwiftGuiStdApplication : public BlackGui::CGuiApplication
{
    Q_OBJECT

public:
    //! Constructor
    CSwiftGuiStdApplication(int argc, char *argv[]);

    //! Single instance
    CSwiftGuiStdApplication *instance();

    //! Start facade by cmd arguments
    void startCoreFacade();

protected:
    //! Parsing of special CMD args
    virtual void parsingHookIn() override;

private:
    QCommandLineOption m_cmdFacadeMode
    {
        { "c" , "core" },
        QCoreApplication::translate("main", "Core mode: (e)xternal, (g)ui (in GUI process), (l)ocalaudio (external, but local audio)."),
        "coremode"
    };  //!< Facade startup mode
};

#endif // guard
