// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MINICOCKPITWIDGET_H
#define SWIFT_GUI_COMPONENTS_MINICOCKPITWIDGET_H

#include <QFrame>
#include <QObject>

#include "gui/swiftguiexport.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/aviation/transponder.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/network/textmessage.h"
#include "misc/network/textmessagelist.h"
#include "misc/simulation/simulatedaircraft.h"

class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;

namespace swift::gui
{
    class CTextMessageTextEdit;
}
namespace swift::gui::components
{
    /*!
     * Represents COM1, COM2, XPDR, and transponder mode as a radio stack
     * (as in a real cockpit): per radio an active/standby frequency display
     * (LCD style) plus a swap button, plus an XPDR code editor and
     * transponder mode buttons (STBY/ON/ALT/IDENT), a SELCAL panel with two
     * selection fields (code pairs), and below that a full-width chat window
     * (text messages) with source selection (All/UNICOM/COM1/COM2) and a
     * single-line input field for sending.
     * Completely without a dependency on CDockWidgetInfoArea / CInfoArea,
     * analogous to CMiniAtcStationWidget / CMiniRadarWidget / CMiniClientsWidget.
     * \remark requires sGui with IContextOwnAircraft / IContextNetwork started
     */
    class SWIFT_GUI_EXPORT CMiniCockpitWidget : public QFrame, public swift::misc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMiniCockpitWidget(QWidget *parent = nullptr);

        //! Destructor
        ~CMiniCockpitWidget() override;

    private:
        //! Builds the layout for a COM panel (COM1 or COM2)
        struct SComPanel
        {
            QLabel *activeLabel = nullptr;
            QLabel *standbyLabel = nullptr;
            QPushButton *swapButton = nullptr;
        };

        //! Chat source (analogous to swiftguistandard: All/UNICOM/COM1/COM2)
        enum ChatSource
        {
            ChatAll,
            ChatUnicom,
            ChatCom1,
            ChatCom2
        };

        //! Creates a COM panel (radio stack look)
        SComPanel createComPanel(QWidget *parent, const QString &title,
                                 swift::misc::aviation::CComSystem::ComUnit unit);

        //! Creates the XPDR panel (code + mode buttons)
        void createXpdrPanel(QWidget *parent);

        //! Creates the SELCAL panel (two selection fields for the code pairs)
        void createSelcalPanel(QWidget *parent);

        //! Creates the chat panel (text messages) below the displays
        void createChatPanel(QWidget *parent);

        //! Updates the display from the current own aircraft
        void updateFromContext(const swift::misc::simulation::CSimulatedAircraft &ownAircraft,
                               const swift::misc::CIdentifier &originator);

        //! Forces an initial update from the context
        void forceUpdateFromContext();

        //! Swaps active/standby frequency of a COM unit
        void swapComFrequency(swift::misc::aviation::CComSystem::ComUnit unit);

        //! Set standby frequency via dialog
        void editStandbyFrequency(swift::misc::aviation::CComSystem::ComUnit unit);

        //! XPDR code was changed in the GUI
        void onXpdrCodeEdited();

        //! Transponder mode button was clicked
        void setTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode);

        //! Updates the visual highlighting of the mode buttons
        void highlightTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode);

        //! SELCAL selection field was changed
        void onSelcalChanged();

        //! Returns the currently selected SELCAL
        swift::misc::aviation::CSelcal getSelcal() const;

        //! Sets the SELCAL selection fields
        void setSelcal(const swift::misc::aviation::CSelcal &selcal);

        //! Text was confirmed in the input field (Enter) -> send
        void onChatTextEntered();

        //! Display received text messages (filtered and sorted by source)
        void onTextMessagesReceived(const swift::misc::network::CTextMessageList &messages);

        //! Display own sent text message
        void onTextMessageSent(const swift::misc::network::CTextMessage &sentMessage);

        //! Currently selected chat source
        ChatSource currentChatSource() const;

        //! Frequency for the currently selected source (invalid for private/all)
        swift::misc::physical_quantities::CFrequency frequencyForSource(ChatSource source) const;

        //! Formats a frequency for the LCD display (e.g. "122.800")
        static QString formatFrequency(const swift::misc::physical_quantities::CFrequency &frequency);

        SComPanel m_com1;
        SComPanel m_com2;

        QLineEdit *m_xpdrCodeEdit = nullptr;
        QPushButton *m_xpdrStandbyButton = nullptr;
        QPushButton *m_xpdrOnButton = nullptr;
        QPushButton *m_xpdrAltButton = nullptr;
        QPushButton *m_xpdrIdentButton = nullptr;

        QComboBox *m_selcalPair1 = nullptr;
        QComboBox *m_selcalPair2 = nullptr;

        QComboBox *m_chatSourceSelector = nullptr;
        swift::gui::CTextMessageTextEdit *m_chatTextEdit = nullptr;
        QLineEdit *m_chatInputEdit = nullptr;

        swift::misc::aviation::CTransponder::TransponderMode m_currentMode =
            swift::misc::aviation::CTransponder::StateStandby;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MINICOCKPITWIDGET_H