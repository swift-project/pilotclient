/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_RAWFSDMESSAGESCOMPONENT_H
#define BLACKGUI_RAWFSDMESSAGESCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/connectionguard.h"

#include <QFrame>

class QAction;
class QPoint;
class QWidget;

namespace Ui { class CRawFsdMessagesComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! GUI displaying raw FSD messages
        class BLACKGUI_EXPORT CRawFsdMessagesComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CRawFsdMessagesComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CRawFsdMessagesComponent();

        private:
            void changeWritingToFile(bool enable);
            void selectFileDir();
            void changeFileWritingMode();
            void setFileWritingModeFromSettings(BlackCore::Vatsim::CRawFsdMessageSettings::FileWriteMode mode);
            void addFsdMessage(const BlackMisc::Network::CRawFsdMessage &rawFsdMessage);
            void readSettings();

            BlackMisc::CSetting<BlackCore::Vatsim::TRawFsdMessageSetting> m_setting { this };

            QScopedPointer<Ui::CRawFsdMessagesComponent> ui;
            BlackMisc::CConnectionGuard m_signalConnections;  //!< connected signal/slots
        };
    } // ns
} // ns
#endif // guard
