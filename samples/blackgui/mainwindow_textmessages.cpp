#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;

/*
 * Text messages received or send, append to GUI
 */
void MainWindow::appendTextMessagesToGui(const CTextMessageList &messages, bool sending)
{
    if (messages.isEmpty()) return;
    foreach(CTextMessage message, messages)
    {
        bool relevantForMe = false;
        QString m = message.asString(true, true, "\t");
        m = message.asString(true, false, "\t");

        if (message.isSendToUnicom())
        {
            this->ui->te_TextMessagesUnicom->append(m);
            relevantForMe = true;
        }

        // check message
        if (message.isRadioMessage())
        {
            // check for own COM frequencies
            if (message.isSendToFrequency(this->m_ownAircraft.getCom1System().getFrequencyActive()))
            {
                this->ui->te_TextMessagesCOM1->append(m);
                relevantForMe = true;
            }
            if (message.isSendToFrequency(this->m_ownAircraft.getCom2System().getFrequencyActive()))
            {
                this->ui->te_TextMessagesCOM2->append(m);
                relevantForMe = true;
            }
        }
        else if (message.isPrivateMessage() && !message.isServerMessage())
        {
            // private message
            this->addPrivateChannelTextMessage(message, sending);
            relevantForMe = true;
        }

        // message for me? right frequency? otherwise quit
        if (relevantForMe || message.isServerMessage()) this->ui->te_TextMessagesAll->append(m);
        if (!relevantForMe) return;

        // overlay message if this channel is not selected
        if (!sending && !message.isSendToUnicom() && !message.isServerMessage())
        {
            // if the channel is selected, do nothing
            if (!this->isCorrespondingTextMessageTabSelected(message))
                this->displayOverlayInfo(m);
        }
    }
}

/*
 * Is the tab of the message's receiver selected?
 */
bool MainWindow::isCorrespondingTextMessageTabSelected(CTextMessage textMessage) const
{
    if (!this->isMainPageSelected(MainPageTextMessages)) return false;
    if (!textMessage.hasValidRecipient()) return false;
    if (textMessage.isEmpty()) return false; // ignore empty message
    if (textMessage.isPrivateMessage())
    {
        // private message
        CCallsign cs = textMessage.getSenderCallsign();
        if (cs.isEmpty()) return false;
        QWidget *tab = this->findTextMessageTabByName(cs.getStringAsSet());
        if (!tab) return false;
        return this->ui->tw_TextMessages->currentWidget() == tab;
    }
    else
    {
        // frequency message
        if (this->ui->tw_TextMessages->currentWidget() == this->ui->tb_TextMessagesAll) return true;
        if (textMessage.isSendToFrequency(this->m_ownAircraft.getCom1System().getFrequencyActive()))
            return this->ui->tw_TextMessages->currentWidget() == this->ui->tb_TextMessagesCOM1;
        if (textMessage.isSendToFrequency(this->m_ownAircraft.getCom2System().getFrequencyActive()))
            return this->ui->tw_TextMessages->currentWidget() == this->ui->tb_TextMessagesCOM2;
        return false;
    }
}


/*
 * Add new text message tab
 */
QWidget *MainWindow::addNewTextMessageTab(const QString &tabName)
{
    QWidget *newTab = new QWidget(this->ui->tw_TextMessages);
    QPushButton *closeButton = new QPushButton("Close", newTab);
    QVBoxLayout *layout = new QVBoxLayout(newTab);
    QTextEdit *textEdit = new QTextEdit(newTab);
    int marginLeft, marginRight, marginTop, marginBottom;
    this->ui->tb_TextMessagesAll->layout()->getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);
    newTab->layout()->setContentsMargins(marginLeft, marginTop, marginRight, 2);
    textEdit->setReadOnly(true);
    textEdit->setWordWrapMode(QTextOption::NoWrap);
    layout->addWidget(textEdit);
    layout->addWidget(closeButton);
    newTab->setLayout(layout);
    int index = this->ui->tw_TextMessages->addTab(newTab, tabName);
    this->connect(closeButton, SIGNAL(released()), this, SLOT(closeTextMessageTab()));
    this->ui->tw_TextMessages->setCurrentIndex(index);
    return newTab;
}

/*
 * Add a private channel text message
 */
void MainWindow::addPrivateChannelTextMessage(const CTextMessage &textMessage, bool sending)
{
    if (!textMessage.isPrivateMessage()) return;
    CCallsign cs = sending ? textMessage.getRecipientCallsign() : textMessage.getSenderCallsign();
    if (cs.isEmpty()) return;
    QWidget *tab = this->findTextMessageTabByName(cs.getStringAsSet());
    if (tab == nullptr) tab = this->findTextMessageTabByName(cs.asString());
    if (tab == nullptr) tab = this->addNewTextMessageTab(cs.getStringAsSet());
    Q_ASSERT(tab != nullptr);
    QTextEdit *textEdit = tab->findChild<QTextEdit *>();
    Q_ASSERT(textEdit != nullptr);
    if (textEdit == nullptr) return; // do not crash, though this situation could not happen
    textEdit->append(textMessage.asString(true, false, "\t"));
}

/*
 * Message tab by name
 */
QWidget *MainWindow::findTextMessageTabByName(const QString &name) const
{
    if (name.isEmpty()) return nullptr;
    QString n = name.trimmed();
    for (int index = 0; index < this->ui->tw_TextMessages->count(); index++)
    {
        QString tabName = this->ui->tw_TextMessages->tabText(index);
        if (tabName.indexOf(n, 0, Qt::CaseInsensitive) < 0) continue;
        QWidget *tab = this->ui->tw_TextMessages->widget(index);
        return tab;
    }
    return nullptr;
}

/*
 * Text message stub (sender/receiver) for current channel
 */
CTextMessage MainWindow::getTextMessageStubForChannel()
{
    CTextMessage tm;
    int index = this->ui->tw_TextMessages->currentIndex();
    if (index < 0) return tm;
    if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesAll)) return tm;

    // from
    tm.setSenderCallsign(this->m_ownAircraft.getCallsign());

    // frequency text message?
    if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM1))
    {
        tm.setFrequency(this->m_ownAircraft.getCom1System().getFrequencyActive());
    }
    else if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesCOM2))
    {
        tm.setFrequency(this->m_ownAircraft.getCom2System().getFrequencyActive());
    }
    else if (index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesUnicom))
    {
        tm.setFrequency(CPhysicalQuantitiesConstants::FrequencyUnicom());
    }
    else
    {
        // not a standard channel
        QString selectedTabText = this->ui->tw_TextMessages->tabText(index);
        bool isNumber;
        double frequency = selectedTabText.toDouble(&isNumber);
        if (isNumber)
        {
            CFrequency radioFrequency = CFrequency(frequency, CFrequencyUnit::MHz());
            if (CComSystem::isValidCivilAviationFrequency(radioFrequency))
            {
                tm.setFrequency(radioFrequency);
            }
            else
            {
                CCallsign toCallsign(selectedTabText);
                tm.setRecipientCallsign(toCallsign);
            }
        }
        else
        {
            CCallsign toCallsign(selectedTabText);
            tm.setRecipientCallsign(toCallsign);
        }
    }
    return tm; // now valid message stub with receiver
}

/*
 * Close message tab
 */
void MainWindow::closeTextMessageTab()
{
    QObject *sender = QObject::sender();
    QWidget *parentWidget = qobject_cast<QWidget *>(sender->parent());
    int index = -1;

    while (index < 0 && parentWidget)
    {
        index =  this->ui->tw_TextMessages->indexOf(parentWidget);
        parentWidget = parentWidget->parentWidget();
    }
    if (index >= 0) this->ui->tw_TextMessages->removeTab(index);
}

/*
 * Command entered
 */
void MainWindow::commandEntered()
{
    // TODO: just a first draft of the command line parser
    // needs to be refactored, as soon as a first version works

    QString cmdLine = this->ui->le_CommandLineInput->text().simplified();
    if (cmdLine.isEmpty()) return;
    QList<QString> parts = cmdLine.toLower().split(' ');
    if (parts.length() < 1) return;
    QString cmd = parts[0].startsWith('.') ? parts[0].toLower() : "";
    if (cmd == ".m" || cmd == ".msg")
    {
        if (!this->m_contextNetworkAvailable || !this->m_contextNetwork->isConnected())
        {
            this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "network needs to be connected"));
            return;
        }
        if (parts.length() < 3)
        {
            this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeValidation, CStatusMessage::SeverityError, "incorrect message"));
            return;
        }
        QString p = parts[1].trimmed(); // receiver

        // select current tab by command
        this->setMainPage(MainPageTextMessages);
        if (p == "c1" || p == "com1")
        {
            this->ui->tw_TextMessages->setCurrentWidget(this->ui->tb_TextMessagesCOM1);
        }
        else if (p == "c2" || p == "com2")
        {
            this->ui->tw_TextMessages->setCurrentWidget(this->ui->tb_TextMessagesCOM2);
        }
        else if (p == "u" || p == "unicom" || p == "uni")
        {
            this->ui->tw_TextMessages->setCurrentWidget(this->ui->tb_TextMessagesUnicom);
        }
        else
        {
            QWidget *tab = this->findTextMessageTabByName(p.trimmed());
            if (tab == nullptr) tab = this->addNewTextMessageTab(p.trimmed().toUpper());
            this->ui->tw_TextMessages->setCurrentWidget(tab);
        }
        CTextMessage tm = this->getTextMessageStubForChannel();
        int index = cmdLine.indexOf(tm.getRecipientCallsign().getStringAsSet(), 0, Qt::CaseInsensitive);
        if (index < 0)
        {
            this->displayStatusMessage(
                CStatusMessage(CStatusMessage::TypeValidation, CStatusMessage::SeverityError,
                               "incomplete message")
            );
            return;
        }
        QString msg(cmdLine.mid(index + tm.getRecipientCallsign().asString().length() + 1));
        tm.setMessage(msg);
        if (tm.isEmpty()) return;
        if (!this->isContextNetworkAvailableCheck()) return;
        CTextMessageList tml(tm);
        this->m_contextNetwork->sendTextMessages(tml);
        this->appendTextMessagesToGui(tml, true);
        this->ui->le_CommandLineInput->setText("");
    }
    else if (cmd.startsWith("."))
    {
        // dump CMDs
    }
    else
    {
        // single line, no command
        // line is considered to be a message to the selected channel, send
        if (!this->m_contextNetwork->isConnected())
        {
            this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "network needs to be connected"));
            return;
        }

        if (!this->isMainPageSelected(MainPageTextMessages))
        {
            this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "text messages can only be sent from corresponding page"));
            return;
        }

        int index = this->ui->tw_TextMessages->currentIndex();
        if (index < 0 || index == this->ui->tw_TextMessages->indexOf(this->ui->tb_TextMessagesAll))
        {
            this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeValidation, CStatusMessage::SeverityError, "incorrect channel"));
        }
        else
        {
            CTextMessage tm = this->getTextMessageStubForChannel();
            tm.setMessage(cmdLine);
            if (tm.isEmpty()) return;
            if (!this->isContextNetworkAvailableCheck()) return;
            CTextMessageList textMessageList(tm);
            this->m_contextNetwork->sendTextMessages(textMessageList);
            this->appendTextMessagesToGui(textMessageList, true);
            this->ui->le_CommandLineInput->setText("");
        }
    }
}
