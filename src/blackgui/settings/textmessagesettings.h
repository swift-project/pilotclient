/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_TEXTMESSAGESETTINGS_H
#define BLACKGUI_SETTINGS_TEXTMESSAGESETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackGui::Settings, CTextMessageSettings)

namespace BlackMisc
{
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
    namespace Network
    {
        class CTextMessage;
    }
}
namespace BlackGui::Settings
{
    //! Text message settings
    class BLACKGUI_EXPORT CTextMessageSettings :
        public BlackMisc::CValueObject<CTextMessageSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexPopupPrivateMessages = BlackMisc::CPropertyIndexRef::GlobalIndexCTextMessageSettings,
            IndexPopupSupervisorMessages,
            IndexPopupFrequencyMessages,
            IndexPopupAllMessages,
            IndexPopupSelcalMessages,
            IndexFocus,
            IndexLatestFirst,
            IndexStyle
        };

        //! Default constructor
        CTextMessageSettings();

        //! Private messages? All considered.
        bool popupPrivateMessages() const;

        //! Private messages
        bool getPopupPrivateMessages() const { return m_popupPrivateMessages; }

        //! Private messages?
        void setPopupPrivateMessages(bool popup) { m_popupPrivateMessages = popup; }

        //! Supervisor messages? All considered.
        bool popupSupervisorMessages() const;

        //! Supervisor messages?
        bool getPopupSupervisorMessages() const { return m_popupSupervisorMessages; }

        //! Supervisor messages?
        void setSupervisorMessages(bool popup) { m_popupSupervisorMessages = popup; }

        //! Frequency messages tuned in?
        bool popupFrequencyMessages() const;

        //! Frequency messages tuned in?
        bool getPopupFrequencyMessages() const { return m_popupFrequencyMessages; }

        //! Frequency messages?
        void setPopupFrequencyMessages(bool popup) { m_popupFrequencyMessages = popup; }

        //! Popup all messages?
        bool getPopupAllMessages() const { return m_popupAllMessages; }

        //! All messages?
        void setPopupAllMessages(bool popup) { m_popupAllMessages = popup; }

        //! Popup SELCAL messages?
        bool getPopupSelcalMessages() const { return m_popupSelcalMessages; }

        //! Popup SELCAL messages? All considered.
        bool popupSelcalMessages() const;

        //! SELCAL messages?
        void setPopupSelcalMessages(bool popup) { m_popupSelcalMessages = popup; }

        //! Entirely disable
        void disableAllPopups();

        //! Popup the given message? Quick check without frequency checks.
        bool popup(const BlackMisc::Network::CTextMessage &textMessage) const;

        //! Popup the given message? Complete check including frequencies.
        bool popup(const BlackMisc::Network::CTextMessage &textMessage, const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft) const;

        //! Latest messages 1st?
        bool isLatestFirst() const { return m_latestFirst; }

        //! Latest messages 1st?
        void setLatestFirst(bool latestFirst) { m_latestFirst = latestFirst; }

        //! CSS style sheet
        const QString &getStyleSheet() const { return m_styleSheet; }

        //! CSS style sheet
        void setStyleSheet(const QString &styleSheet) { m_styleSheet = styleSheet; }

        //! Focus in the overlay window
        bool focusOverlayWindow() const { return m_focus; }

        //! Set focus in overlay window
        void setFocusOverlayWindows(bool focus) { m_focus = focus; }

        //! Reset style sheet
        void resetStyleSheet() { m_styleSheet.clear(); }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

    private:
        bool m_popupPrivateMessages = true;
        bool m_popupSupervisorMessages = true;
        bool m_popupFrequencyMessages = false; //!< show if tuned in
        bool m_popupAllMessages = false;
        bool m_popupSelcalMessages = true;
        bool m_focus = true;
        bool m_latestFirst = false; //!< latest messages first
        QString m_styleSheet;

        BLACK_METACLASS(
            CTextMessageSettings,
            BLACK_METAMEMBER(popupPrivateMessages),
            BLACK_METAMEMBER(popupSupervisorMessages),
            BLACK_METAMEMBER(popupFrequencyMessages),
            BLACK_METAMEMBER(popupAllMessages),
            BLACK_METAMEMBER(popupSelcalMessages),
            BLACK_METAMEMBER(focus),
            BLACK_METAMEMBER(latestFirst),
            BLACK_METAMEMBER(styleSheet)
        );
    };

    //! Text message settings
    struct TextMessageSettings : public BlackMisc::TSettingTrait<CTextMessageSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "textmessages"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Text message");
            return name;
        }
    };
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CTextMessageSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CTextMessageSettings>)

#endif // guard
