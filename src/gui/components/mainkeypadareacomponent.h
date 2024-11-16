// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MAINKEYPADAREACOMPONENT_H
#define SWIFT_GUI_MAINKEYPADAREACOMPONENT_H

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/components/maininfoareacomponent.h"
#include "gui/swiftguiexport.h"
#include "misc/identifier.h"
#include "misc/network/connectionstatus.h"

class QPushButton;

namespace swift::misc::simulation
{
    class CSimulatedAircraft;
}
namespace Ui
{
    class CMainKeypadAreaComponent;
}
namespace swift::gui::components
{
    //! Main keypad area as used with main info area
    //! \sa CMainInfoAreaComponent
    class SWIFT_GUI_EXPORT CMainKeypadAreaComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMainKeypadAreaComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CMainKeypadAreaComponent() override;

        //! Main info area changed
        void onMainInfoAreaChanged(int currentTabIndex, const QList<int> &dockedIndexes,
                                   const QList<int> &floatingIndexes);

        //! Focus in entry fields
        void focusInEntryField();

    signals:
        //! Button to select main info area has been pressed
        //! \sa CMainInfoAreaComponent
        void selectedMainInfoAreaDockWidget(CMainInfoAreaComponent::InfoArea infoArea);

        //! Change opacity 0..30
        void changedOpacity(int opacity);

        //! \copydoc CCommandInput::commandEntered
        void commandEntered(const QString &commandLine, const swift::misc::CIdentifier &originator);

        //! \copydoc CCommandInput::commandEntered
        void textEntered(const QString &commandLine, const swift::misc::CIdentifier &originator);

        //! Connect was pressed
        void connectPressed();

        //! Ident pressed
        void identPressed();

        //! Request audio
        void audioPressed();

    private:
        //! Button was clicked
        void buttonSelected();

        //! \copydoc swift::core::context::IContextNetwork::connectionStatusChanged
        void connectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                     const swift::misc::network::CConnectionStatus &to);

        //! \copydoc swift::core::context::IContextOwnAircraft::changedAircraftCockpit
        void ownAircraftCockpitChanged(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                       const swift::misc::CIdentifier &originator);

        //! \copydoc swift::core::context::IContextAudio::changedOutputMute
        void outputMuteChanged(bool muted);

        //! If button is info area, identify it
        CMainInfoAreaComponent::InfoArea buttonToMainInfoArea(const QObject *button) const;

        //! Main info area to corresponding button
        QPushButton *mainInfoAreaToButton(CMainInfoAreaComponent::InfoArea area) const;

        //! Info area buttons
        void unsetInfoAreaButtons();

        //! Update values
        void update();

        //! Update connection status
        void updateConnectionStatus();

        QScopedPointer<Ui::CMainKeypadAreaComponent> ui;
        swift::misc::CIdentifier m_identifier { "MainKeypadAreaComponent", this };
    };
} // namespace swift::gui::components

#endif // guard
