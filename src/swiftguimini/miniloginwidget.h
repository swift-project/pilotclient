// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINILOGINWIDGET_H
#define SWIFTGUIMINI_MINILOGINWIDGET_H

#include "gui/components/logincomponent.h"

//! Abgeleitete, isolierte Variante von CLoginComponent (nur fuer die Mini-GUI),
//! die den Login-Button auch ohne aktive Simulatorverbindung aktiviert laesst.
//! \remarks CLoginComponent selbst bleibt unveraendert (wird auch von swiftguistandard genutzt).
class CMiniLoginWidget : public swift::gui::components::CLoginComponent
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniLoginWidget(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniLoginWidget() override;

private:
    //! Erzwingt den aktivierten Zustand des Login/Logoff-Buttons,
    //! unabhaengig vom Simulatorverbindungsstatus.
    void forceEnableLoginButton();
};

#endif // SWIFTGUIMINI_MINILOGINWIDGET_H