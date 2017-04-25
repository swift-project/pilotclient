/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_APPLICATIONCLOSEDIALOG_H
#define BLACKGUI_COMPONENTS_APPLICATIONCLOSEDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include <QAbstractButton>

namespace Ui { class CApplicationCloseDialog; }
namespace BlackGui
{
    namespace Components
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

        private slots:
            //! Accepted
            void ps_onAccepted();

            //! Rejected
            void ps_onRejected();

            //! Button pressed
            void ps_buttonClicked(QAbstractButton *button);

        private:
            QScopedPointer<Ui::CApplicationCloseDialog> ui;
            QStringList m_settingskeys;

            //! Init the settings view
            void initSettingsView();
        };
    } // ns
} // ns
#endif // guard
