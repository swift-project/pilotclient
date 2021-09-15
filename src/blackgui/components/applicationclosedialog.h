/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_APPLICATIONCLOSEDIALOG_H
#define BLACKGUI_COMPONENTS_APPLICATIONCLOSEDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include <QAbstractButton>
#include "blackcore/context/contextapplication.h"

namespace Ui { class CApplicationCloseDialog; }
namespace BlackGui::Components
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
        BlackCore::Context::CSettingsDictionary m_settingsDictionary; //!< values and descriptions, unsorted

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
