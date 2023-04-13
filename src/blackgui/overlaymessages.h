/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_OVERLAYMESSAGES_H
#define BLACKGUI_OVERLAYMESSAGES_H

#include "blackgui/components/textmessagecomponenttab.h"
#include "blackgui/settings/textmessagesettings.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QMessageBox>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>
#include <functional>

class QKeyEvent;
class QPaintEvent;
class QPixmap;

namespace Ui
{
    class COverlayMessages;
}
namespace BlackMisc::Network
{
    class CTextMessage;
}
namespace BlackGui
{
    /*!
     * Display status messages (nested in the parent widget)
     */
    class BLACKGUI_EXPORT COverlayMessages : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessages(int w, int h, QWidget *parent);

        //! Destructor
        virtual ~COverlayMessages() override;

        //! Messages mode
        void setModeToMessages(bool withKillButton = false);

        //! Single Message mode
        void setModeToMessage(bool withKillButton = false);

        //! Single Message mode
        void setModeToMessageSmall(bool withKillButton = false);

        //! HTML message mode
        void setModeToHTMLMessage(bool withKillButton = false);

        //! Progress bar
        void setModeToProgressBar(bool withKillButton = false);

        //! Single text message mode
        void setModeToTextMessage();

        //! Inline text message
        void setModeToOverlayTextMessage();

        //! Active send/receive of text messages
        void activateTextMessages(bool activate);

        //! Are text messages
        bool isTextMessagesActivated() const;

        //! Display image
        void setModeToImage();

        //! Force small (smaller layout)
        void setForceSmall(bool force) { m_forceSmall = force; }

        //! Set header text
        void setHeaderText(const QString &header);

        //! Set the message and show the confirmation frame
        void setConfirmationMessage(const QString &message);

        //! Show multiple messages with confirmation bar
        void showOverlayMessagesWithConfirmation(
            const BlackMisc::CStatusMessageList &messages,
            bool appendOldMessages,
            const QString &confirmationMessage,
            std::function<void()> okLambda,
            QMessageBox::StandardButton defaultButton = QMessageBox::Cancel,
            int timeOutMs = -1);

        //! Clear the overlay messages
        void clearOverlayMessages();

        //! Show multiple messages
        void showOverlayMessages(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1);

        //! Show multiple messages or a single message
        void showOverlayMessagesOrSingleMessage(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1);

        //! Show multiple messages or a single message (HTML)
        void showOverlayMessagesOrHTMLMessage(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1);

        //! Sort of overlay messages
        void sortOverlayMessages(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

        //! Set sorting of overlay messages
        void setOverlayMessagesSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

        //! Show single message
        void showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1);

        //! Info message, based on text message
        void showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs = -1);

        //! Inline text message
        void showOverlayInlineTextMessage(Components::TextMessageTab tab);

        //! Inline text message
        void showOverlayInlineTextMessage(const BlackMisc::Aviation::CCallsign &callsign);

        //! Image
        void showOverlayImage(const BlackMisc::CPixmap &image, int timeOutMs = -1);

        //! Image
        void showOverlayImage(const QPixmap &image, int timeOutMs = -1);

        //! Display one of the supported types
        void showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs = -1);

        //! HTML message
        void showHTMLMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1);

        //! HTML message
        void showHTMLMessage(const QString &htmlMessage, int timeOutMs = -1);

        //! Download progress
        void showDownloadProgress(int progress, qint64 current, qint64 max, const QUrl &url, int timeOutMs = -1);

        //! Progress bar
        void showProgressBar(int percentage, const QString &message, int timeOutMs = -1);

        //! Allows to globally enable/disable kill button
        void showKillButton(bool killButton);

        //! Close button clicked
        void close();

        //! Set the default confirmation button
        void setDefaultConfirmationButton(int button = QMessageBox::Cancel);

        //! Is awaiting a conformation
        bool hasPendingConfirmation() const;

        //! Display reduced information
        void setReducedInfo(bool reduced);

        //! Add a shadow
        void addShadow(QColor color = Qt::gray);

    signals:
        //! Confirmation completed
        void confirmationCompleted();

    protected:
        //! Show message
        void display(int timeOutMs = -1);

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

    private:
        QScopedPointer<Ui::COverlayMessages> ui;
        BlackMisc::CSettingReadOnly<Settings::TextMessageSettings> m_messageSettings { this };
        QString m_header;
        int m_lastConfirmation = QMessageBox::Cancel;
        bool m_awaitingConfirmation = false;
        bool m_hasKillButton = false;
        bool m_forceSmall = false;
        QTimer m_autoCloseTimer { this };
        std::function<void()> m_okLambda; //!< called when confirmed as "OK"
        QList<std::function<void()>> m_pendingMessageCalls;

        //! Stylesheet changed
        void onStyleSheetsChanged();

        //! OK clicked (only when confirmation bar is active)
        void onOkClicked();

        //! Cancel clicked (only when confirmation bar is active)
        void onCancelClicked();

        //! Kill clicked (only when errors)
        void onKillClicked();

        //! Init widget
        void init(int w, int h);

        //! Set header text
        void setHeader(const QString &header);

        //! Small
        bool useSmall() const;

        //! Show kill button?
        //! \sa COverlayMessages::showKillButton globally enable/disable kill button
        void showKill(bool show);

        //! Display this message (use settings to decide)
        bool displayTextMessage(const BlackMisc::Network::CTextMessage &textMessage) const;
    };
} // ns

#endif // guard
