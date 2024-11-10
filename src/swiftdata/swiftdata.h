// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTDATA_H
#define SWIFTDATA_H

#include "blackgui/settings/guisettings.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/managedstatusbar.h"
#include "blackcore/db/backgrounddataupdater.h"
#include "misc/loghistory.h"
#include "misc/identifiable.h"
#include "misc/statusmessage.h"

#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CSwiftData;
}
namespace BlackCore
{
    class CWebDataServices;
}
namespace BlackGui::Components
{
    class CAutoPublishDialog;
}

/*!
 * swift data entry control (aka mapping tool)
 */
class CSwiftData :
    public QMainWindow,
    public swift::misc::CIdentifiable,
    public BlackGui::IMainWindowAccess
{
    Q_OBJECT
    Q_INTERFACES(BlackGui::IMainWindowAccess)

public:
    //! Constructor
    CSwiftData(QWidget *parent = nullptr);

    //! Destructor
    virtual ~CSwiftData() override;

protected:
    //! \name QMainWindow events
    //! @{

    //! \copydoc QMainWindow::closeEvent
    virtual void closeEvent(QCloseEvent *event) override;
    //! @}

private:
    //! Style sheet has changed
    void onStyleSheetsChanged();

    //! Menu clicked
    void onMenuClicked();

    //! @{
    //! Init functions
    void init();
    void initLogDisplay();
    void initStyleSheet();
    void initMenu();
    void initDynamicMenus();
    //! @}

    //! Check auto-publishing
    void checkAutoPublishing();

    //! Show auto publishing
    void showAutoPublishing();

    void performGracefulShutdown();
    void consolidationSettingChanged();
    void displayLog();
    void checkMinimumVersion();

    QScopedPointer<Ui::CSwiftData> ui;
    BlackGui::CManagedStatusBar m_statusBar;
    BlackCore::Db::CBackgroundDataUpdater *m_updater = nullptr; //!< consolidate with DB data
    swift::misc::CSettingReadOnly<BlackGui::Settings::TBackgroundConsolidation> m_consolidationSettings { this, &CSwiftData::consolidationSettingChanged }; //!< consolidation time
    swift::misc::CLogHistoryReplica m_logHistory { this };

    // auto update
    BlackGui::Components::CAutoPublishDialog *m_autoPublishDialog = nullptr; //!< auto publishing dialog
};

#endif // guard
