// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_UPDATEINFODIALOG_H
#define SWIFT_GUI_COMPONENTS_UPDATEINFODIALOG_H

#include <QDialog>

#include "gui/settings/updatenotification.h"
#include "gui/swiftguiexport.h"

namespace Ui
{
    class CUpdateInfoDialog;
}
namespace swift::gui::components
{
    /*!
     * Download and install swift
     */
    class SWIFT_GUI_EXPORT CUpdateInfoDialog : public QDialog
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
        swift::misc::CSetting<swift::gui::settings::TUpdateNotificationSettings> m_setting { this }; //!< show again?

        //! Toggled checkbox
        void onDontShowAgain(bool dontShowAgain);

        //! Selection in distribution component changed
        void selectionChanged();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_UPDATEINFODIALOG_H
