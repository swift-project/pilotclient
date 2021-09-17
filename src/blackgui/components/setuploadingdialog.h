/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETUPLOADINGDIALOG_H
#define BLACKGUI_COMPONENTS_SETUPLOADINGDIALOG_H

#include "blackmisc/statusmessagelist.h"
#include <QDialog>

namespace Ui { class CSetupLoadingDialog; }
namespace BlackGui::Components
{
    class CCopyModelsFromOtherSwiftVersionsDialog;

    /*!
     * Setup dialog, if something goes wrong allows to copy bootstrap file
     */
    class CSetupLoadingDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CSetupLoadingDialog(QWidget *parent = nullptr);

        //! Ctor with messages
        CSetupLoadingDialog(const BlackMisc::CStatusMessageList &msgs, QWidget *parent = nullptr);

        //! Dtor
        virtual ~CSetupLoadingDialog();

    private:
        QScopedPointer<Ui::CSetupLoadingDialog> ui;
        QScopedPointer<CCopyModelsFromOtherSwiftVersionsDialog> m_copyFromOtherSwiftVersion;

        //! Cached setup available?
        bool hasCachedSetup() const;

        //! Setup reader?
        bool hasSetupReader() const;

        //! Set info fields
        void displayBootstrapUrls();

        //! Display bootstrap URL
        void displayCmdBoostrapUrl();

        //! Display global setup
        void displayGlobalSetup();

        //! Open the help page
        void openHelpPage();

        //! Try again without explicit bootstrap URL
        void tryAgainWithoutBootstrapUrl();

        //! Try to fix
        void tryToFix();

        //! Prefill setup cache
        void prefillSetupCache();

        //! Display the setup cache info
        void displaySetupCacheInfo();

        //! Display other versions info
        void displayOtherVersionsInfo();

        //! Open directory
        void openDirectory();

        //! Copy from other swift versions
        void copyFromOtherSwiftVersions();

        //! Setup loading has been completed
        void onSetupHandlingCompleted(bool success);
    };
} // ns

#endif // guard
