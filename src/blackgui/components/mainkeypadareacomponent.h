/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAINKEYPADAREACOMPONENT_H
#define BLACKGUI_MAINKEYPADAREACOMPONENT_H

#include "blackcore/network.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/components/maininfoareacomponent.h"
#include "blackmisc/identifier.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QPushButton;
class QWidget;

namespace BlackMisc { namespace Simulation { class CSimulatedAircraft; } }
namespace Ui { class CMainKeypadAreaComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Main keypad area as used with main info area
        //! \sa CMainInfoAreaComponent
        class BLACKGUI_EXPORT CMainKeypadAreaComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMainKeypadAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMainKeypadAreaComponent();

        signals:
            //! Button to select main info area has been pressed
            //! \sa CMainInfoAreaComponent
            void selectedMainInfoAreaDockWidget(CMainInfoAreaComponent::InfoArea infoArea);

            //! Change opacity 0..30
            void changedOpacity(int opacity);

            //! \copydoc CCommandInput::commandEntered
            void commandEntered(const QString &commandLine, const BlackMisc::CIdentifier &originator);

            //! \copydoc CCommandInput::commandEntered
            void textEntered(const QString &commandLine, const BlackMisc::CIdentifier &originator);

            //! Connect was pressed
            void connectPressed();

            //! Ident pressed
            void identPressed();

            //! Request audio
            void audioPressed();

        public slots:
            //! Main info area changed
            void onMainInfoAreaChanged(int currentTabIndex, QList<int> dockedIndexes, QList<int> floatingIndexes);

        private:
            //! Button was clicked
            void buttonSelected();

            //! \copydoc BlackCore::Context::IContextNetwork::connectionStatusChanged
            void connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! \copydoc BlackCore::Context::IContextOwnAircraft::changedAircraftCockpit
            void ownAircraftCockpitChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! \copydoc BlackCore::Context::IContextAudio::changedMute
            void muteChanged(bool muted);

            //! If button is info area, identify it
            CMainInfoAreaComponent::InfoArea buttonToMainInfoArea(const QObject *button) const;

            //! Main info area to corresponding button
            QPushButton *mainInfoAreaToButton(CMainInfoAreaComponent::InfoArea area) const;

            //! Info area buttons
            void unsetInfoAreaButtons();

            //! Disable button for some (short) time to avoid double click
            void disableButtonBriefly();

            QScopedPointer<Ui::CMainKeypadAreaComponent> ui;
            BlackMisc::CIdentifier m_identifier { "MainKeypadAreaComponent", this };
        };
    } // namespace
} // namespace

#endif // guard
