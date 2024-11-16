// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTDATA_H
#define SWIFTDATA_H

#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>

#include "core/db/backgrounddataupdater.h"
#include "gui/mainwindowaccess.h"
#include "gui/managedstatusbar.h"
#include "gui/settings/guisettings.h"
#include "misc/identifiable.h"
#include "misc/loghistory.h"
#include "misc/statusmessage.h"

namespace Ui
{
    class CSwiftData;
}
namespace swift::core
{
    class CWebDataServices;
}
namespace swift::gui::components
{
    class CAutoPublishDialog;
}

/*!
 * swift data entry control (aka mapping tool)
 */
class CSwiftData :
    public QMainWindow,
    public swift::misc::CIdentifiable,
    public swift::gui::IMainWindowAccess
{
    Q_OBJECT
    Q_INTERFACES(swift::gui::IMainWindowAccess)

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
    swift::gui::CManagedStatusBar m_statusBar;
    swift::core::db::CBackgroundDataUpdater *m_updater = nullptr; //!< consolidate with DB data
    swift::misc::CSettingReadOnly<swift::gui::settings::TBackgroundConsolidation> m_consolidationSettings { this, &CSwiftData::consolidationSettingChanged }; //!< consolidation time
    swift::misc::CLogHistoryReplica m_logHistory { this };

    // auto update
    swift::gui::components::CAutoPublishDialog *m_autoPublishDialog = nullptr; //!< auto publishing dialog
};

#endif // guard
