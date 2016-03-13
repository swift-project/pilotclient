/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTDATA_H
#define SWIFTDATA_H

#include "blackgui/systemtraywindow.h"
#include "blackgui/managedstatusbar.h"
#include "blackgui/mainwindowaccess.h"
#include "blackcore/corefacade.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/updateinfo.h"
#include "blackcore/setupreader.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/identifiable.h"
#include <QScopedPointer>

namespace Ui { class CSwiftData; }
namespace BlackCore { class CWebDataServices; }

/*!
 * swift data entry control (aka mapping tool)
 */
class CSwiftData :
    public QMainWindow,
    public BlackMisc::CIdentifiable,
    public BlackGui::IMainWindowAccess
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

public:
    //! Constructor
    CSwiftData(QWidget *parent = nullptr);

    //! Destructor
    ~CSwiftData();

private slots:
    //! Append log message
    void ps_appendLogMessage(const BlackMisc::CStatusMessage &message);

    //! Style sheet has changed
    void ps_onStyleSheetsChanged();

    //! Menu clicked
    void ps_onMenuClicked();

private:
    void init();
    void initLogDisplay();
    void initStyleSheet();
    void initMenu();
    void initDynamicMenus();
    void performGracefulShutdown();

    void displayConsole();
    void displayLog();

    QScopedPointer<Ui::CSwiftData> ui;
    BlackGui::CManagedStatusBar    m_statusBar;
    BlackMisc::CData<BlackCore::Data::UpdateInfo>  m_updateInfo { this };  //!< download / version data
};

#endif // guard
