// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_UPDATEINFODIALOG_H
#define BLACKGUI_COMPONENTS_UPDATEINFODIALOG_H

#include "blackgui/settings/updatenotification.h"
#include "blackgui/blackguiexport.h"
#include <QDialog>

namespace Ui
{
    class CUpdateInfoDialog;
}
namespace BlackGui::Components
{
    /*!
     * Download and install swift
     */
    class BLACKGUI_EXPORT CUpdateInfoDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CUpdateInfoDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CUpdateInfoDialog() override;

        //! A new version existing?
        bool isNewVersionAvailable() const;

        //! \copydoc QDialog::exec
        virtual int exec() override;

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

    private:
        QScopedPointer<Ui::CUpdateInfoDialog> ui;
        BlackMisc::CSetting<BlackGui::Settings::TUpdateNotificationSettings> m_setting { this }; //!< show again?

        //! Toggled checkbox
        void onDontShowAgain(bool dontShowAgain);

        //! Selection in distribution component changed
        void selectionChanged();
    };
} // ns

#endif // guard
