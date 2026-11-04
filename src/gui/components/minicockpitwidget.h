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
     * Stellt COM1, COM2, XPDR und Transponder-Mode als Abbildung eines
     * Radio-Stacks (wie im echten Cockpit) dar: je Funkgeraet aktive/Standby
     * Frequenz-Anzeige (LCD-Stil) plus Swap-Knopf, dazu ein XPDR-Code-Editor
     * und Transponder-Mode-Knoepfe (STBY/ON/ALT/IDENT), ein SELCAL-Panel
     * mit zwei Auswahlfeldern (Code-Paaren) sowie darunter ein ueber die
     * volle Breite gehendes Chatfenster (Textmessages) mit Quellenauswahl
     * (All/UNICOM/COM1/COM2) und einzeiligem Eingabefeld zum Senden.
     * Komplett ohne Abhaengigkeit zu CDockWidgetInfoArea / CInfoArea,
     * analog zu CMiniAtcStationWidget / CMiniRadarWidget / CMiniClientsWidget.
     * \remark benoetigt sGui mit gestartetem IContextOwnAircraft / IContextNetwork
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
        //! Baut das Layout fuer ein COM-Panel (COM1 oder COM2) auf
        struct SComPanel
        {
            QLabel *activeLabel = nullptr;
            QLabel *standbyLabel = nullptr;
            QPushButton *swapButton = nullptr;
        };

        //! Quelle des Chats (analog zu swiftguistandard: All/UNICOM/COM1/COM2)
        enum ChatSource
        {
            ChatAll,
            ChatUnicom,
            ChatCom1,
            ChatCom2
        };

        //! Erzeugt ein COM-Panel (Radio-Stack-Optik)
        SComPanel createComPanel(QWidget *parent, const QString &title,
                                 swift::misc::aviation::CComSystem::ComUnit unit);

        //! Erzeugt das XPDR-Panel (Code + Mode-Knoepfe)
        void createXpdrPanel(QWidget *parent);

        //! Erzeugt das SELCAL-Panel (zwei Auswahlfelder fuer die Code-Paare)
        void createSelcalPanel(QWidget *parent);

        //! Erzeugt das Chat-Panel (Textmessages) unterhalb der Anzeigen
        void createChatPanel(QWidget *parent);

        //! Aktualisiert die Anzeige aus dem aktuellen eigenen Flugzeug
        void updateFromContext(const swift::misc::simulation::CSimulatedAircraft &ownAircraft,
                               const swift::misc::CIdentifier &originator);

        //! Erzwingt ein initiales Update aus dem Kontext
        void forceUpdateFromContext();

        //! Tauscht aktive/Standby Frequenz eines COM-Geraets
        void swapComFrequency(swift::misc::aviation::CComSystem::ComUnit unit);

        //! Standby-Frequenz per Dialog setzen
        void editStandbyFrequency(swift::misc::aviation::CComSystem::ComUnit unit);

        //! XPDR Code wurde in der GUI geaendert
        void onXpdrCodeEdited();

        //! Transponder-Mode-Knopf wurde geklickt
        void setTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode);

        //! Aktualisiert die optische Hervorhebung der Mode-Knoepfe
        void highlightTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode);

        //! SELCAL-Auswahlfeld wurde geaendert
        void onSelcalChanged();

        //! Liefert das aktuell ausgewaehlte SELCAL
        swift::misc::aviation::CSelcal getSelcal() const;

        //! Setzt die SELCAL-Auswahlfelder
        void setSelcal(const swift::misc::aviation::CSelcal &selcal);

        //! Text wurde im Eingabefeld bestaetigt (Enter) -> senden
        void onChatTextEntered();

        //! Empfangene Textnachrichten anzeigen (nach Quelle gefiltert einsortieren)
        void onTextMessagesReceived(const swift::misc::network::CTextMessageList &messages);

        //! Eigene gesendete Textnachricht anzeigen
        void onTextMessageSent(const swift::misc::network::CTextMessage &sentMessage);

        //! Aktuell gewaehlte Chat-Quelle
        ChatSource currentChatSource() const;

        //! Frequenz fuer die aktuell gewaehlte Quelle (ungueltig bei private/all)
        swift::misc::physical_quantities::CFrequency frequencyForSource(ChatSource source) const;

        //! Formatiert eine Frequenz fuer die LCD-Anzeige (z.B. "122.800")
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