/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

    /*!
     * Multi purpose info window (pop up window)
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
        QTimer *m_hideTimer;

        //! Init the window
        void initWindow();

        //! Show window, hide after some time
        void showWindow(int displayTimeMs);

        //! Current page
        void setCurrentPage(QWidget *widget);
    };
}

#endif // guard
