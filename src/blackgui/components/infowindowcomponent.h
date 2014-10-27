/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INFOWINDOW_H
#define BLACKGUI_INFOWINDOW_H

#include "blackmisc/nwtextmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"

#include <QWizardPage>
#include <QScopedPointer>

namespace Ui { class InfoWindow; }

namespace BlackGui
{
    namespace Components
    {

        /*!
         * Multi purpose info window (like pop up window)
         */
        class CInfoWindowComponent : public QWizardPage
        {
            Q_OBJECT

        public:
            const static int DefaultDisplayTimeMs = 4000; //!< Display n milliseconds

            //! Constructor
            explicit CInfoWindowComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CInfoWindowComponent();

        public slots:

            //! Info message, pure string
            void displayStringMessage(const QString &message, int displayTimeMs = DefaultDisplayTimeMs);

            //! Info message, based on text message
            void displayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int displayTimeMs = DefaultDisplayTimeMs);

            //! Info message, based on status message
            void displayStatusMessage(const BlackMisc::CStatusMessage &statusMessage, int displayTimeMs = DefaultDisplayTimeMs);

            //! Display any of the specialized types
            void display(const BlackMisc::CVariant &variant, int displayTimeMs = DefaultDisplayTimeMs);

        private:
            QScopedPointer<Ui::InfoWindow> ui; //!< user interface
            QTimer *m_hideTimer = nullptr;

            //! Init the window
            void initWindow();

            //! Show window, hide after some time
            void showWindow(int displayTimeMs);

            //! Current page
            void setCurrentPage(QWidget *widget);

        private slots:
            //! Style sheet changed
            void onStyleSheetsChanged();
        };
    } // namespace
} // namespace
#endif // guard
