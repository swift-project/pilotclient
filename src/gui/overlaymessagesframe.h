// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_OVERLAYMESSAGES_FRAME_H
#define SWIFT_GUI_OVERLAYMESSAGES_FRAME_H

#include <functional>

#include <QDockWidget>
#include <QFrame>
#include <QKeyEvent>
#include <QMessageBox>
#include <QObject>
#include <QSize>
#include <QString>
#include <QTabWidget>
#include <QTableView>
#include <QTreeView>
#include <QUrl>
#include <QWizardPage>

#include "gui/components/textmessagecomponenttab.h"
#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/guiutility.h"
#include "gui/overlaymessages.h"
#include "gui/swiftguiexport.h"
#include "misc/logmessage.h"
#include "misc/network/textmessage.h"
#include "misc/pixmap.h"
#include "misc/statusmessagelist.h"
#include "misc/variant.h"

class QKeyEvent;
class QPaintEvent;

namespace swift::gui
{
    /*!
     * Base class to display overlay messages in different widgets
     * (nested in this widget).
     * \fixme KB 2017-12 all header version, if someone manages to create a cpp version go ahead, I failed on gcc with
     * "undefined reference to `swift::gui::COverlayMessagesBase<QFrame>::showOverlayMessages`"
     */
    template <class WIDGET>
    class COverlayMessagesBase : public WIDGET
    {
    public:
        //! Destructor
        virtual ~COverlayMessagesBase() override {}

        //! Init, normally we use lazy init, but by calling init explicitly we can force initalization
        //! \remark usefule for text messages, as history will be already available
        void initOverlayMessages(QSize inner = {})
        {
            if (m_overlayMessages) { return; }
            if (inner.isNull()) { inner = this->innerFrameSize(); }

            m_overlayMessages = new COverlayMessages(inner.width(), inner.height(), this);
            m_overlayMessages->hide();
            m_overlayMessages->showKillButton(m_showKillButton);
            m_overlayMessages->setForceSmall(m_forceSmallMsgs);
            m_overlayMessages->setReducedInfo(m_reducedInfo);
        }

        //! \copydoc swift::gui::COverlayMessages::activateTextMessages
        void activateTextMessages(bool activate)
        {
            this->initOverlayMessages();
            m_overlayMessages->activateTextMessages(activate);
        }

        //! Show the inner frame
        void showStatusMessagesFrame() { this->initInnerFrame(); }

        //! Set the size factors
        void setOverlaySizeFactors(double widthFactor, double heightFactor, double middleFactor = 2)
        {
            m_widthFactor = widthFactor;
            m_heightFactor = heightFactor;
            if (middleFactor >= 0) { m_middleFactor = middleFactor; }
        }

        //! \copydoc swift::gui::COverlayMessages::showKillButton
        void showKillButton(bool killButton)
        {
            m_showKillButton = killButton;
            if (m_overlayMessages) { m_overlayMessages->showKillButton(killButton); }
        }

        //! \copydoc swift::gui::COverlayMessages::setForceSmall
        void setForceSmall(bool force)
        {
            m_forceSmallMsgs = force;
            if (m_overlayMessages) { m_overlayMessages->setForceSmall(force); }
        }

        //! \copydoc swift::gui::COverlayMessages::setReducedInfo
        void setReducedInfo(bool reduced)
        {
            m_reducedInfo = reduced;
            if (m_overlayMessages) { m_overlayMessages->setReducedInfo(reduced); }
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayMessagesWithConfirmation
        void showOverlayMessagesWithConfirmation(const swift::misc::CStatusMessageList &messages,
                                                 bool appendOldMessages, const QString &confirmationMessage,
                                                 std::function<void()> okLambda,
                                                 QMessageBox::StandardButton defaultButton = QMessageBox::Cancel,
                                                 int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            if (!this->hasMinimumSize(150, 150)) { return; }
            m_overlayMessages->showOverlayMessagesWithConfirmation(messages, appendOldMessages, confirmationMessage,
                                                                   okLambda, defaultButton, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::clearOverlayMessages
        void clearOverlayMessages()
        {
            if (!m_overlayMessages) { return; }
            m_overlayMessages->clearOverlayMessages();
        }

        //! \copydoc swift::gui::COverlayMessages::close
        void closeOverlay()
        {
            if (!m_overlayMessages) { return; }
            m_overlayMessages->close();
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayMessages
        void showOverlayMessages(const swift::misc::CStatusMessageList &messages, bool appendOldMessages = false,
                                 int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessages(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayMessagesOrSingleMessage
        void showOverlayMessagesOrSingleMessage(const swift::misc::CStatusMessageList &messages,
                                                bool appendOldMessages = false, int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessagesOrSingleMessage(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayMessagesOrHTMLMessage
        void showOverlayMessagesOrHTMLMessage(const swift::misc::CStatusMessageList &messages,
                                              bool appendOldMessages = false, int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessagesOrHTMLMessage(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::sortOverlayMessages
        void sortOverlayMessages(const swift::misc::CPropertyIndex &property, Qt::SortOrder order)
        {
            m_overlayMessages->sortOverlayMessages(property, order);
        }

        //! \copydoc swift::gui::COverlayMessages::setOverlayMessagesSorting
        void setOverlayMessagesSorting(const swift::misc::CPropertyIndex &property, Qt::SortOrder order)
        {
            m_overlayMessages->setOverlayMessagesSorting(property, order);
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayMessage
        bool showOverlayMessage(const swift::misc::CStatusMessage &message, int timeOutMs = -1)
        {
            if (message.isEmpty()) { return false; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessage(message, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayTextMessage
        bool showOverlayTextMessage(const swift::misc::network::CTextMessage &textMessage, int timeOutMs = -1)
        {
            if (textMessage.isEmpty()) { return false; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayTextMessage(textMessage, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayVariant
        void showOverlayVariant(const swift::misc::CVariant &variant, int timeOutMs = -1)
        {
            if (m_overlayMessages->isTextMessagesActivated() &&
                variant.canConvert<swift::misc::network::CTextMessage>())
            {
                this->initInnerFrame(0.75, 0.75);
                if (!this->hasMinimumSize(150, 150)) { return; }
            }
            else { this->initInnerFrame(); }

            m_overlayMessages->showOverlayVariant(variant, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayImage
        void showOverlayImage(const swift::misc::CPixmap &pixmap, int timeOutMs = -1)
        {
            this->initInnerFrame();
            m_overlayMessages->showOverlayImage(pixmap, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::showHTMLMessage
        bool showOverlayHTMLMessage(const QString &htmlMessage, int timeOutMs = -1)
        {
            this->initMinimalFrame();
            m_overlayMessages->showHTMLMessage(htmlMessage, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc swift::gui::COverlayMessages::showHTMLMessage
        bool showOverlayHTMLMessage(const swift::misc::CStatusMessage &message, int timeOutMs = -1)
        {
            this->initMinimalFrame();
            m_overlayMessages->showHTMLMessage(message, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc swift::gui::COverlayMessages::showDownloadProgress
        void showDownloadProgress(int progress, qint64 current, qint64 max, const QUrl &url, int timeOutMs = -1)
        {
            this->initMinimalFrame();
            m_overlayMessages->showDownloadProgress(progress, current, max, url, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayImage
        void showOverlayInlineTextMessage(components::TextMessageTab tab)
        {
            this->initInnerFrame(0.75, 0.75);
            if (!this->hasMinimumSize(150, 150)) { return; }
            m_overlayMessages->showOverlayInlineTextMessage(tab);
            WIDGET::repaint();
        }

        //! \copydoc swift::gui::COverlayMessages::showOverlayImage
        void showOverlayInlineTextMessage(const swift::misc::aviation::CCallsign &callsign)
        {
            this->initInnerFrame(0.75, 0.75);
            if (!this->hasMinimumSize(150, 150)) { return; }
            m_overlayMessages->showOverlayInlineTextMessage(callsign);
            WIDGET::repaint();
        }

    protected:
        COverlayMessages *m_overlayMessages = nullptr; //!< embedded QFrame with status messages

        //! Constructor
        COverlayMessagesBase(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags()) : WIDGET(parent)
        {
            this->setWindowFlags(f);
            const bool isFrameless = CGuiUtility::isMainWindowFrameless();
            m_middleFactor = isFrameless ? 1.25 : 1.5; // 2 is middle in normal window
        }

        //! Init the inner frame (if not yet initialized)
        void initInnerFrame(double widthFactor = -1, double heightFactor = -1)
        {
            const QSize inner(innerFrameSize(widthFactor, heightFactor));
            if (!m_overlayMessages)
            {
                // lazy init
                this->initOverlayMessages(inner);
            }

            Q_ASSERT(m_overlayMessages);

            const QPoint middle = WIDGET::geometry().center();
            const int w = inner.width();
            const int h = inner.height();
            const int x = middle.x() - w / 2;
            const int y = qRound(middle.y() - h / m_middleFactor);

            m_overlayMessages->setGeometry(x, y, w, h);
            m_overlayMessages->setVisible(true);
        }

        //! Init a minimal frame (smaller as the normal one)
        void initMinimalFrame(int lines = 4)
        {
            this->initInnerFrame();

            // get logical resolution
            constexpr int MinHeight = 100;
            QSizeF s = CGuiUtility::fontMetricsEstimateSize(100, lines, true); // 2 lines for header
            if (s.height() < MinHeight) { s.setHeight(MinHeight); }

            const QSize inner(innerFrameSize());
            const QPoint middle = WIDGET::geometry().center();
            const int w = qMin(inner.width(), qRound(s.width()));
            const int h = qMin(inner.height(), qRound(s.height()));
            const int x = middle.x() - w / 2;
            const int y = qRound(middle.y() - h / m_middleFactor);
            m_overlayMessages->setGeometry(x, y, w, h);
        }

        //! Check minimum height/width
        bool hasMinimumSize(int w, int h) const
        {
            if (w > 0 && m_overlayMessages->width() < w)
            {
                swift::misc::CLogMessage(this).info(u"Overlay widget too small (w)");
                return false;
            }

            if (h > 0 && m_overlayMessages->height() < h)
            {
                swift::misc::CLogMessage(this).info(u"Overlay widget too small (h)");
                return false;
            }
            return true;
        }

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override
        {
            if (m_overlayMessages && event->key() == Qt::Key_Escape)
            {
                m_overlayMessages->close();
                event->accept();
            }
            else { WIDGET::keyPressEvent(event); }
        }

        //! \copydoc QFrame::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override
        {
            WIDGET::resizeEvent(event);
            if (m_overlayMessages && m_overlayMessages->isVisible()) { this->initInnerFrame(); }
        }

    private:
        //! Calculate inner frame size
        QSize innerFrameSize(double widthFactor = -1, double heightFactor = -1) const
        {
            // check against minimum if widget is initialized, but not yet resized
            const int w = std::max(WIDGET::width(), WIDGET::minimumWidth());
            const int h = std::max(WIDGET::height(), WIDGET::minimumHeight());

            widthFactor = qMin(widthFactor < 0 ? m_widthFactor : widthFactor, 0.95);
            heightFactor = qMin(heightFactor < 0 ? m_heightFactor : heightFactor, 0.95);

            int wInner = qRound(widthFactor * w);
            int hInner = qRound(heightFactor * h);
            if (wInner > WIDGET::maximumWidth()) { wInner = WIDGET::maximumWidth(); }
            if (hInner > WIDGET::maximumHeight()) { hInner = WIDGET::maximumHeight(); }
            return QSize(wInner, hInner);
        }

        bool m_showKillButton = false; //!< show kill button
        bool m_forceSmallMsgs = false; //!< force small messages
        bool m_reducedInfo = false; //!< reduced info (no timestamp ..)
        double m_widthFactor = 0.7; //!< inner frame x factor
        double m_heightFactor = 0.6; //!< inner frame x factor
        double m_middleFactor = 2; //!< 2 means middle, 1 means on top
    };

    /*!
     * Using this class provides a QFrame with the overlay functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesFrame : public COverlayMessagesBase<QFrame>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    signals:
        //! Request an text message entry
        void requestTextMessageEntryTab(components::TextMessageTab tab);

        //! Request an text message entry
        void requestTextMessageEntryCallsign(const swift::misc::aviation::CCallsign &callsign);

        //! Request a text message widget
        void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);
    };

    /*!
     * Using this class provides a QFrame with the overlay and dock widget functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesFrameEnableForDockWidgetInfoArea :
        public COverlayMessagesFrame,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesFrameEnableForDockWidgetInfoArea(QWidget *parent = nullptr,
                                                                  Qt::WindowFlags f = Qt::WindowFlags());

    private:
        //! Forward overlay messages
        bool isForwardingOverlayMessages() const;
    };

    /*!
     * Using this class provides a QTabWidget with the overlay functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesTabWidget : public COverlayMessagesBase<QTabWidget>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTabWidget(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QWizardPage with the overlay functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesWizardPage : public COverlayMessagesBase<QWizardPage>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesWizardPage(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QDockWidget with the overlay functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesDockWidget : public COverlayMessagesBase<QDockWidget>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesDockWidget(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QTableView with the overlay functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesTableView : public COverlayMessagesBase<QTableView>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTableView(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QTableView with the overlay functionality already integrated.
     */
    class SWIFT_GUI_EXPORT COverlayMessagesTreeView : public COverlayMessagesBase<QTreeView>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTreeView(QWidget *parent = nullptr);
    };

} // namespace swift::gui

#endif // guard
