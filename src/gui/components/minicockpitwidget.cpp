// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/minicockpitwidget.h"

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "gui/textmessagetextedit.h"
#include "misc/pq/constants.h"
#include "misc/pq/units.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CMiniCockpitWidget::CMiniCockpitWidget(QWidget *parent) : QFrame(parent), CIdentifiable(this)
    {
        this->setObjectName("CMiniCockpitWidget");

        QVBoxLayout *outer = new QVBoxLayout(this);

        QFrame *stack = new QFrame(this);
        stack->setObjectName("radioStackPanel");
        QHBoxLayout *stackLayout = new QHBoxLayout(stack);

        m_com1 = this->createComPanel(stack, "COM1", CComSystem::Com1);
        m_com2 = this->createComPanel(stack, "COM2", CComSystem::Com2);
        this->createXpdrPanel(stack);
        this->createSelcalPanel(stack);

        stackLayout->addStretch(1);
        outer->addWidget(stack);

        // Chatfenster ueber die volle Breite, unterhalb der Anzeigen
        this->createChatPanel(this);

        this->forceUpdateFromContext();

        if (sGui && sGui->getIContextOwnAircraft())
        {
            connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this,
                    &CMiniCockpitWidget::updateFromContext, Qt::QueuedConnection);
            connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedSelcal, this,
                    [this](const CSelcal &selcal, const CIdentifier &originator) {
                        if (this->isMyIdentifier(originator)) { return; }
                        this->setSelcal(selcal);
                    });

            QPointer<CMiniCockpitWidget> myself(this);
            QTimer::singleShot(2 * 1000, this, [=, this] {
                if (!sGui || sGui->isShuttingDown() || !myself) { return; }
                this->forceUpdateFromContext();
            });
        }

        if (sGui && sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::textMessagesReceived, this,
                    &CMiniCockpitWidget::onTextMessagesReceived, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::textMessageSent, this,
                    &CMiniCockpitWidget::onTextMessageSent, Qt::QueuedConnection);
        }
    }

    CMiniCockpitWidget::~CMiniCockpitWidget() = default;

    CMiniCockpitWidget::SComPanel CMiniCockpitWidget::createComPanel(QWidget *parent, const QString &title,
                                                                     CComSystem::ComUnit unit)
    {
        SComPanel panel;

        QFrame *radio = new QFrame(parent);
        radio->setFrameShape(QFrame::StyledPanel);
        QVBoxLayout *radioLayout = new QVBoxLayout(radio);

        QLabel *titleLabel = new QLabel(title, radio);
        titleLabel->setObjectName("cockpitPanelTitle");
        titleLabel->setAlignment(Qt::AlignHCenter);
        radioLayout->addWidget(titleLabel);

        panel.activeLabel = new QLabel("---.---", radio);
        panel.activeLabel->setObjectName("lcdActiveLabel");
        panel.activeLabel->setAlignment(Qt::AlignCenter);
        radioLayout->addWidget(panel.activeLabel);

        QHBoxLayout *standbyRow = new QHBoxLayout();
        panel.standbyLabel = new QLabel("---.---", radio);
        panel.standbyLabel->setObjectName("lcdStandbyLabel");
        panel.standbyLabel->setAlignment(Qt::AlignCenter);
        panel.standbyLabel->setCursor(Qt::PointingHandCursor);
        panel.standbyLabel->setToolTip("Doppelklick: Standby-Frequenz eingeben");
        standbyRow->addWidget(panel.standbyLabel, 1);

        panel.swapButton = new QPushButton(QStringLiteral("\u21C6"), radio); // ⇆
        panel.swapButton->setToolTip("Aktive und Standby-Frequenz tauschen");
        panel.swapButton->setFixedWidth(32);
        standbyRow->addWidget(panel.swapButton);

        radioLayout->addLayout(standbyRow);

        connect(panel.swapButton, &QPushButton::clicked, this, [this, unit] { this->swapComFrequency(unit); });

        // Doppelklick auf Standby-Anzeige -> Frequenz per Dialog setzen
        panel.standbyLabel->installEventFilter(this);
        connect(panel.standbyLabel, &QLabel::linkActivated, this,
                [] {}); // placeholder, label has no native dblclick signal

        // workaround: use a lambda-based mouse-press via event filter is more work; instead use a button beneath for
        // editing
        QPushButton *editButton = new QPushButton("Standby setzen...", radio);
        connect(editButton, &QPushButton::clicked, this, [this, unit] { this->editStandbyFrequency(unit); });
        radioLayout->addWidget(editButton);

        static_cast<QHBoxLayout *>(parent->layout())->addWidget(radio);
        return panel;
    }

    void CMiniCockpitWidget::createXpdrPanel(QWidget *parent)
    {
        QFrame *xpdr = new QFrame(parent);
        xpdr->setFrameShape(QFrame::StyledPanel);
        QVBoxLayout *xpdrLayout = new QVBoxLayout(xpdr);

        QLabel *titleLabel = new QLabel("XPDR", xpdr);
        titleLabel->setObjectName("cockpitPanelTitle");
        titleLabel->setAlignment(Qt::AlignHCenter);
        xpdrLayout->addWidget(titleLabel);

        m_xpdrCodeEdit = new QLineEdit(xpdr);
        m_xpdrCodeEdit->setObjectName("lcdActiveLineEdit");
        m_xpdrCodeEdit->setInputMask("9999");
        m_xpdrCodeEdit->setAlignment(Qt::AlignCenter);
        m_xpdrCodeEdit->setMaxLength(4);
        xpdrLayout->addWidget(m_xpdrCodeEdit);
        connect(m_xpdrCodeEdit, &QLineEdit::editingFinished, this, &CMiniCockpitWidget::onXpdrCodeEdited);

        QGridLayout *modeGrid = new QGridLayout();
        m_xpdrStandbyButton = new QPushButton("STBY", xpdr);
        m_xpdrOnButton = new QPushButton("ON", xpdr);
        m_xpdrAltButton = new QPushButton("ALT", xpdr);
        m_xpdrIdentButton = new QPushButton("IDENT", xpdr);

        modeGrid->addWidget(m_xpdrStandbyButton, 0, 0);
        modeGrid->addWidget(m_xpdrOnButton, 0, 1);
        modeGrid->addWidget(m_xpdrAltButton, 1, 0);
        modeGrid->addWidget(m_xpdrIdentButton, 1, 1);
        xpdrLayout->addLayout(modeGrid);

        connect(m_xpdrStandbyButton, &QPushButton::clicked, this,
                [this] { this->setTransponderMode(CTransponder::StateStandby); });
        connect(m_xpdrOnButton, &QPushButton::clicked, this, [this] { this->setTransponderMode(CTransponder::ModeA); });
        connect(m_xpdrAltButton, &QPushButton::clicked, this,
                [this] { this->setTransponderMode(CTransponder::ModeC); });
        connect(m_xpdrIdentButton, &QPushButton::clicked, this,
                [this] { this->setTransponderMode(CTransponder::StateIdent); });

        static_cast<QHBoxLayout *>(parent->layout())->addWidget(xpdr);
    }

    void CMiniCockpitWidget::createSelcalPanel(QWidget *parent)
    {
        QFrame *selcal = new QFrame(parent);
        selcal->setFrameShape(QFrame::StyledPanel);
        QVBoxLayout *selcalLayout = new QVBoxLayout(selcal);

        QLabel *titleLabel = new QLabel("SELCAL", selcal);
        titleLabel->setObjectName("cockpitPanelTitle");
        titleLabel->setAlignment(Qt::AlignHCenter);
        selcalLayout->addWidget(titleLabel);

        QHBoxLayout *comboRow = new QHBoxLayout();
        m_selcalPair1 = new QComboBox(selcal);
        m_selcalPair2 = new QComboBox(selcal);
        m_selcalPair1->addItems(CSelcal::codePairs());
        m_selcalPair2->addItems(CSelcal::codePairs());
        comboRow->addWidget(m_selcalPair1);
        comboRow->addWidget(m_selcalPair2);
        selcalLayout->addLayout(comboRow);

        connect(m_selcalPair1, &QComboBox::currentTextChanged, this, [this] { this->onSelcalChanged(); });
        connect(m_selcalPair2, &QComboBox::currentTextChanged, this, [this] { this->onSelcalChanged(); });

        static_cast<QHBoxLayout *>(parent->layout())->addWidget(selcal);
    }

    void CMiniCockpitWidget::createChatPanel(QWidget *parent)
    {
        QFrame *chat = new QFrame(parent);
        chat->setFrameShape(QFrame::StyledPanel);
        QVBoxLayout *chatLayout = new QVBoxLayout(chat);

        QHBoxLayout *headerRow = new QHBoxLayout();
        QLabel *titleLabel = new QLabel("Chat", chat);
        titleLabel->setObjectName("cockpitPanelTitle");
        headerRow->addWidget(titleLabel);

        QLabel *sourceLabel = new QLabel("Quelle:", chat);
        headerRow->addWidget(sourceLabel);

        m_chatSourceSelector = new QComboBox(chat);
        m_chatSourceSelector->addItem("All", ChatAll);
        m_chatSourceSelector->addItem("UNICOM", ChatUnicom);
        m_chatSourceSelector->addItem("COM1", ChatCom1);
        m_chatSourceSelector->addItem("COM2", ChatCom2);
        headerRow->addWidget(m_chatSourceSelector);
        headerRow->addStretch(1);
        chatLayout->addLayout(headerRow);

        m_chatTextEdit = new CTextMessageTextEdit(chat);
        m_chatTextEdit->setMinimumHeight(140);
        chatLayout->addWidget(m_chatTextEdit, 1);

        m_chatInputEdit = new QLineEdit(chat);
        m_chatInputEdit->setPlaceholderText("Text eingeben und Enter druecken zum Senden...");
        chatLayout->addWidget(m_chatInputEdit);

        connect(m_chatInputEdit, &QLineEdit::returnPressed, this, &CMiniCockpitWidget::onChatTextEntered);

        // ueber die volle Breite (horizontale Size-Policy Expanding ist Default bei QFrame in QVBoxLayout)
        qobject_cast<QVBoxLayout *>(parent->layout())->addWidget(chat);
    }

    void CMiniCockpitWidget::updateFromContext(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
    {
        Q_UNUSED(originator) // Anzeige wird auch bei eigenen Aenderungen aktualisiert (reine Anzeige-Labels)

        const CComSystem com1 = ownAircraft.getCom1System();
        const CComSystem com2 = ownAircraft.getCom2System();
        const CTransponder transponder = ownAircraft.getTransponder();

        m_com1.activeLabel->setText(formatFrequency(com1.getFrequencyActive()));
        m_com1.standbyLabel->setText(formatFrequency(com1.getFrequencyStandby()));
        m_com2.activeLabel->setText(formatFrequency(com2.getFrequencyActive()));
        m_com2.standbyLabel->setText(formatFrequency(com2.getFrequencyStandby()));

        if (m_xpdrCodeEdit && !m_xpdrCodeEdit->hasFocus())
        {
            m_xpdrCodeEdit->setText(QString("%1").arg(transponder.getTransponderCode(), 4, 10, QChar('0')));
        }

        m_currentMode = transponder.getTransponderMode();
        this->highlightTransponderMode(m_currentMode);

        this->setSelcal(ownAircraft.getSelcal());
    }

    void CMiniCockpitWidget::forceUpdateFromContext()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
        this->updateFromContext(ownAircraft, CIdentifier("dummyInitialValues"));
    }

    void CMiniCockpitWidget::swapComFrequency(CComSystem::ComUnit unit)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        IContextOwnAircraft *ctx = sGui->getIContextOwnAircraft();

        CComSystem com1 = ctx->getOwnComSystem(CComSystem::Com1);
        CComSystem com2 = ctx->getOwnComSystem(CComSystem::Com2);
        const CTransponder transponder = ctx->getOwnTransponder();

        if (unit == CComSystem::Com1) { com1.toggleActiveStandby(); }
        else { com2.toggleActiveStandby(); }

        ctx->updateCockpit(com1, com2, transponder, identifier());
    }

    void CMiniCockpitWidget::editStandbyFrequency(CComSystem::ComUnit unit)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        IContextOwnAircraft *ctx = sGui->getIContextOwnAircraft();

        const CComSystem current = ctx->getOwnComSystem(unit);
        bool ok = false;
        const double value = QInputDialog::getDouble(
            this, "Standby-Frequenz", "Frequenz in MHz:", current.getFrequencyStandby().value(CFrequencyUnit::MHz()),
            118.0, 137.0, 3, &ok);
        if (!ok) { return; }

        ctx->updateStandbyComFrequency(CFrequency(value, CFrequencyUnit::MHz()), unit, identifier());
    }

    void CMiniCockpitWidget::onXpdrCodeEdited()
    {
        if (!m_xpdrCodeEdit) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }

        // Eingabemaske "9999" kann bei unvollstaendiger Eingabe Leerzeichen enthalten
        // (z.B. "12  " statt "0012") -> diese entfernen, bevor wir parsen
        QString text = m_xpdrCodeEdit->text();
        text.replace(QLatin1Char(' '), QLatin1Char('0'));

        bool ok = false;
        const int code = text.toInt(&ok);
        if (!ok || !CTransponder::isValidTransponderCode(code))
        {
            // ungueltig -> auf aktuellen Kontextwert zuruecksetzen
            if (sGui->getIContextOwnAircraft())
            {
                const int currentCode = sGui->getIContextOwnAircraft()->getOwnTransponder().getTransponderCode();
                m_xpdrCodeEdit->setText(QString("%1").arg(currentCode, 4, 10, QChar('0')));
            }
            return;
        }

        IContextOwnAircraft *ctx = sGui->getIContextOwnAircraft();
        CComSystem com1 = ctx->getOwnComSystem(CComSystem::Com1);
        CComSystem com2 = ctx->getOwnComSystem(CComSystem::Com2);
        CTransponder transponder = ctx->getOwnTransponder();
        transponder.setTransponderCode(code);

        ctx->updateCockpit(com1, com2, transponder, identifier());

        // Anzeige sofort auf formatiertes 4-stelliges Format normalisieren
        m_xpdrCodeEdit->setText(QString("%1").arg(code, 4, 10, QChar('0')));
    }

    void CMiniCockpitWidget::setTransponderMode(CTransponder::TransponderMode mode)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        sGui->getIContextOwnAircraft()->updateTransponderMode(mode, identifier());
        m_currentMode = mode;
        this->highlightTransponderMode(mode);
    }

    void CMiniCockpitWidget::highlightTransponderMode(CTransponder::TransponderMode mode)
    {
        const QString activeStyle = "QPushButton { background-color: #39ff6a; color: #111; font-weight: bold; }";
        const QString normalStyle = "";

        if (m_xpdrStandbyButton)
        {
            m_xpdrStandbyButton->setStyleSheet(mode == CTransponder::StateStandby ? activeStyle : normalStyle);
        }
        if (m_xpdrOnButton) { m_xpdrOnButton->setStyleSheet(mode == CTransponder::ModeA ? activeStyle : normalStyle); }
        if (m_xpdrAltButton)
        {
            m_xpdrAltButton->setStyleSheet(mode == CTransponder::ModeC ? activeStyle : normalStyle);
        }
        if (m_xpdrIdentButton)
        {
            m_xpdrIdentButton->setStyleSheet(mode == CTransponder::StateIdent ? activeStyle : normalStyle);
        }
    }

    void CMiniCockpitWidget::onSelcalChanged()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        const CSelcal selcal = this->getSelcal();
        sGui->getIContextOwnAircraft()->updateSelcal(selcal, identifier());
    }

    CSelcal CMiniCockpitWidget::getSelcal() const
    {
        if (!m_selcalPair1 || !m_selcalPair2) { return {}; }
        QString code = m_selcalPair1->currentText();
        code.append(m_selcalPair2->currentText());
        return CSelcal(code);
    }

    void CMiniCockpitWidget::setSelcal(const CSelcal &selcal)
    {
        if (!m_selcalPair1 || !m_selcalPair2) { return; }
        const QString code = selcal.getCode();
        if (code.length() != 4) { return; } // noch unvollstaendig

        const QString s1 = code.left(2);
        const QString s2 = code.right(2);

        if (CSelcal::codePairs().contains(s1) && m_selcalPair1->currentText() != s1)
        {
            const QSignalBlocker blocker(m_selcalPair1);
            m_selcalPair1->setCurrentText(s1);
        }
        if (CSelcal::codePairs().contains(s2) && m_selcalPair2->currentText() != s2)
        {
            const QSignalBlocker blocker(m_selcalPair2);
            m_selcalPair2->setCurrentText(s2);
        }
    }

    CMiniCockpitWidget::ChatSource CMiniCockpitWidget::currentChatSource() const
    {
        if (!m_chatSourceSelector) { return ChatAll; }
        return static_cast<ChatSource>(m_chatSourceSelector->currentData().toInt());
    }

    CFrequency CMiniCockpitWidget::frequencyForSource(ChatSource source) const
    {
        switch (source)
        {
        case ChatUnicom: return CPhysicalQuantitiesConstants::FrequencyUnicom();
        case ChatCom1:
        {
            if (sGui && sGui->getIContextOwnAircraft())
            {
                return sGui->getIContextOwnAircraft()->getOwnComSystem(CComSystem::Com1).getFrequencyActive();
            }
            return {};
        }
        case ChatCom2:
        {
            if (sGui && sGui->getIContextOwnAircraft())
            {
                return sGui->getIContextOwnAircraft()->getOwnComSystem(CComSystem::Com2).getFrequencyActive();
            }
            return {};
        }
        case ChatAll:
        default: return {};
        }
    }

    void CMiniCockpitWidget::onChatTextEntered()
    {
        if (!m_chatInputEdit) { return; }
        const QString text = m_chatInputEdit->text().trimmed();
        if (text.isEmpty()) { return; }
        m_chatInputEdit->clear();

        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }

        const ChatSource source = this->currentChatSource();
        if (source == ChatAll)
        {
            // "All" ist eine reine Anzeige-Quelle, senden erfordert COM1/COM2/UNICOM
            return;
        }

        const CFrequency frequency = this->frequencyForSource(source);
        if (frequency.isNull()) { return; }

        const CTextMessage message(text, frequency);
        CTextMessageList messages;
        messages.push_back(message);
        sGui->getIContextNetwork()->sendTextMessages(messages);
    }

    void CMiniCockpitWidget::onTextMessagesReceived(const CTextMessageList &messages)
    {
        if (!m_chatTextEdit) { return; }

        const CFrequency freq1 = this->frequencyForSource(ChatCom1);
        const CFrequency freq2 = this->frequencyForSource(ChatCom2);

        for (const CTextMessage &message : messages)
        {
            if (!message.isRadioMessage()) { continue; }

            bool relevant = false;
            if (message.isSendToUnicom()) { relevant = true; }
            if (!freq1.isNull() && message.isSendToFrequency(freq1)) { relevant = true; }
            if (!freq2.isNull() && message.isSendToFrequency(freq2)) { relevant = true; }

            if (relevant) { m_chatTextEdit->insertTextMessage(message); }
        }
    }

    void CMiniCockpitWidget::onTextMessageSent(const CTextMessage &sentMessage)
    {
        if (!m_chatTextEdit) { return; }
        if (!sentMessage.isRadioMessage()) { return; }
        m_chatTextEdit->insertTextMessage(sentMessage);
    }

    QString CMiniCockpitWidget::formatFrequency(const CFrequency &frequency)
    {
        if (frequency.isNull()) { return "---.---"; }
        return QString::number(frequency.value(CFrequencyUnit::MHz()), 'f', 3);
    }
} // namespace swift::gui::components