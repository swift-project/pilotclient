// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_RAWFSDMESSAGESCOMPONENT_H
#define BLACKGUI_RAWFSDMESSAGESCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackmisc/network/rawfsdmessagelist.h"
#include "blackmisc/connectionguard.h"

#include <QFrame>

namespace Ui
{
    class CRawFsdMessagesComponent;
}
namespace BlackGui::Components
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
        void setupConnections();
        void enableDisableRawFsdMessages();
        void expandFilters(bool expand);
        void expandWritingToFile(bool expand);
        void changeStringFilter();
        void changePacketTypeFilter(const QString &type);
        void changeMaxDisplayedMessages();
        void filterDisplayedMessages();
        void selectFileDir();
        void changeFileWritingMode();
        void addFsdMessage(const BlackMisc::Network::CRawFsdMessage &rawFsdMessage);
        void clearAllMessages();
        void readSettings();

        static QString rawFsdMessageToString(const BlackMisc::Network::CRawFsdMessage &rawFsdMessage);

        BlackMisc::CSetting<BlackCore::Vatsim::TRawFsdMessageSetting> m_setting { this };

        QScopedPointer<Ui::CRawFsdMessagesComponent> ui;
        BlackMisc::CConnectionGuard m_signalConnections; //!< connected signal/slots

        QString m_filterString;
        QString m_filterPacketType;

        BlackMisc::Network::CRawFsdMessageList m_buffer;

        int m_maxDisplayedMessages = 100;
        int m_maxBufferSize = 1000;
    };
} // ns
#endif // guard
