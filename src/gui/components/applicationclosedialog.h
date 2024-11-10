// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_APPLICATIONCLOSEDIALOG_H
#define SWIFT_GUI_COMPONENTS_APPLICATIONCLOSEDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include <QAbstractButton>
#include "core/context/contextapplication.h"

namespace Ui
{
    class CApplicationCloseDialog;
}
namespace swift::gui::components
{
    /*!
     * Close dialog for application
     */
    class CApplicationCloseDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CApplicationCloseDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CApplicationCloseDialog();

    private:
        QScopedPointer<Ui::CApplicationCloseDialog> ui;
        QStringList m_settingsDescriptions; //!< values as displayed
        swift::core::context::CSettingsDictionary m_settingsDictionary; //!< values and descriptions, unsorted

        //! Accepted
        void onAccepted();

        //! Rejected
        void onRejected();

        //! Button pressed
        void buttonClicked(QAbstractButton *button);

        //! Init the settings view
        void initSettingsView();
    };
} // ns
#endif // guard
