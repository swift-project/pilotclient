/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef SWIFTDATA_H
#define SWIFTDATA_H

#include "blackgui/settings/guisettings.h"
#include "blackgui/mainwindowaccess.h"
#include "blackgui/managedstatusbar.h"
#include "blackcore/db/backgrounddataupdater.h"
#include "blackmisc/loghistory.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/statusmessage.h"

#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CSwiftData; }
namespace BlackCore { class CWebDataServices; }
namespace BlackGui { namespace Components { class CAutoPublishDialog; }}

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
    virtual ~CSwiftData() override;

protected:
    //! \name QMainWindow events @[
    virtual void closeEvent(QCloseEvent *event) override;
    //! @}

private:
    //! Style sheet has changed
    void onStyleSheetsChanged();

    //! Menu clicked
    void onMenuClicked();

    //! Init functions @{
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
    void displayConsole();
    void displayLog();
    void checkMinimumVersion();

    QScopedPointer<Ui::CSwiftData> ui;
    BlackGui::CManagedStatusBar m_statusBar;
    BlackCore::Db::CBackgroundDataUpdater *m_updater = nullptr; //!< consolidate with DB data
    BlackMisc::CSettingReadOnly<BlackGui::Settings::TBackgroundConsolidation> m_consolidationSettings { this, &CSwiftData::consolidationSettingChanged }; //!< consolidation time
    BlackMisc::CLogHistoryReplica m_logHistory { this };

    // auto update
    BlackGui::Components::CAutoPublishDialog *m_autoPublishDialog = nullptr; //!< auto publishing dialog
};

#endif // guard
