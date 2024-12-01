// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_TEXTMESSAGESETTINGS_H
#define SWIFT_GUI_SETTINGS_TEXTMESSAGESETTINGS_H

#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/settingscache.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::gui::settings, CTextMessageSettings)

namespace swift::misc
{
    namespace simulation
    {
        class CSimulatedAircraft;
    }
    namespace network
    {
        class CTextMessage;
    }
} // namespace swift::misc
namespace swift::gui::settings
{
    //! Text message settings
    class SWIFT_GUI_EXPORT CTextMessageSettings : public swift::misc::CValueObject<CTextMessageSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexPopupPrivateMessages = swift::misc::CPropertyIndexRef::GlobalIndexCTextMessageSettings,
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
        bool popup(const swift::misc::network::CTextMessage &textMessage) const;

        //! Popup the given message? Complete check including frequencies.
        bool popup(const swift::misc::network::CTextMessage &textMessage,
                   const swift::misc::simulation::CSimulatedAircraft &ownAircraft) const;

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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        bool m_popupPrivateMessages = true;
        bool m_popupSupervisorMessages = true;
        bool m_popupFrequencyMessages = false; //!< show if tuned in
        bool m_popupAllMessages = false;
        bool m_popupSelcalMessages = true;
        bool m_focus = true;
        bool m_latestFirst = false; //!< latest messages first
        QString m_styleSheet;

        SWIFT_METACLASS(
            CTextMessageSettings,
            SWIFT_METAMEMBER(popupPrivateMessages),
            SWIFT_METAMEMBER(popupSupervisorMessages),
            SWIFT_METAMEMBER(popupFrequencyMessages),
            SWIFT_METAMEMBER(popupAllMessages),
            SWIFT_METAMEMBER(popupSelcalMessages),
            SWIFT_METAMEMBER(focus),
            SWIFT_METAMEMBER(latestFirst),
            SWIFT_METAMEMBER(styleSheet));
    };

    //! Text message settings
    struct TextMessageSettings : public swift::misc::TSettingTrait<CTextMessageSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "textmessages"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Text message");
            return name;
        }
    };
} // namespace swift::gui::settings

Q_DECLARE_METATYPE(swift::gui::settings::CTextMessageSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::gui::settings::CTextMessageSettings>)

#endif // SWIFT_GUI_SETTINGS_TEXTMESSAGESETTINGS_H
