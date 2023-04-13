/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_OVERLAYMESSAGES_FRAME_H
#define BLACKGUI_OVERLAYMESSAGES_FRAME_H

#include "blackgui/components/textmessagecomponenttab.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/overlaymessages.h"
#include "blackgui/guiutility.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/variant.h"

#include <QKeyEvent>
#include <QFrame>
#include <QTabWidget>
#include <QWizardPage>
#include <QDockWidget>
#include <QTableView>
#include <QTreeView>
#include <QMessageBox>
#include <QObject>
#include <QSize>
#include <QUrl>
#include <QString>
#include <functional>

class QKeyEvent;
class QPaintEvent;

namespace BlackGui
{
    /*!
     * Base class to display overlay messages in different widgets
     * (nested in this widget).
     * \fixme KB 2017-12 all header version, if someone manages to create a cpp version go ahead, I failed on gcc with "undefined reference to `BlackGui::COverlayMessagesBase<QFrame>::showOverlayMessages`"
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

        //! \copydoc BlackGui::COverlayMessages::activateTextMessages
        void activateTextMessages(bool activate)
        {
            this->initOverlayMessages();
            m_overlayMessages->activateTextMessages(activate);
        }

        //! Show the inner frame
        void showStatusMessagesFrame()
        {
            this->initInnerFrame();
        }

        //! Set the size factors
        void setOverlaySizeFactors(double widthFactor, double heightFactor, double middleFactor = 2)
        {
            m_widthFactor = widthFactor;
            m_heightFactor = heightFactor;
            if (middleFactor >= 0) { m_middleFactor = middleFactor; }
        }

        //! \copydoc BlackGui::COverlayMessages::showKillButton
        void showKillButton(bool killButton)
        {
            m_showKillButton = killButton;
            if (m_overlayMessages)
            {
                m_overlayMessages->showKillButton(killButton);
            }
        }

        //! \copydoc BlackGui::COverlayMessages::setForceSmall
        void setForceSmall(bool force)
        {
            m_forceSmallMsgs = force;
            if (m_overlayMessages)
            {
                m_overlayMessages->setForceSmall(force);
            }
        }

        //! \copydoc BlackGui::COverlayMessages::setReducedInfo
        void setReducedInfo(bool reduced)
        {
            m_reducedInfo = reduced;
            if (m_overlayMessages)
            {
                m_overlayMessages->setReducedInfo(reduced);
            }
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessagesWithConfirmation
        void showOverlayMessagesWithConfirmation(
            const BlackMisc::CStatusMessageList &messages,
            bool appendOldMessages,
            const QString &confirmationMessage,
            std::function<void()> okLambda,
            QMessageBox::StandardButton defaultButton = QMessageBox::Cancel,
            int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            if (!this->hasMinimumSize(150, 150)) { return; }
            m_overlayMessages->showOverlayMessagesWithConfirmation(messages, appendOldMessages, confirmationMessage, okLambda, defaultButton, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::clearOverlayMessages
        void clearOverlayMessages()
        {
            if (!m_overlayMessages) { return; }
            m_overlayMessages->clearOverlayMessages();
        }

        //! \copydoc BlackGui::COverlayMessages::close
        void closeOverlay()
        {
            if (!m_overlayMessages) { return; }
            m_overlayMessages->close();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessages
        void showOverlayMessages(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessages(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessagesOrSingleMessage
        void showOverlayMessagesOrSingleMessage(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessagesOrSingleMessage(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessagesOrHTMLMessage
        void showOverlayMessagesOrHTMLMessage(const BlackMisc::CStatusMessageList &messages, bool appendOldMessages = false, int timeOutMs = -1)
        {
            if (messages.isEmpty()) { return; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessagesOrHTMLMessage(messages, appendOldMessages, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::sortOverlayMessages
        void sortOverlayMessages(const BlackMisc::CPropertyIndex &property, Qt::SortOrder order)
        {
            m_overlayMessages->sortOverlayMessages(property, order);
        }

        //! \copydoc BlackGui::COverlayMessages::setOverlayMessagesSorting
        void setOverlayMessagesSorting(const BlackMisc::CPropertyIndex &property, Qt::SortOrder order)
        {
            m_overlayMessages->setOverlayMessagesSorting(property, order);
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayMessage
        bool showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1)
        {
            if (message.isEmpty()) { return false; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayMessage(message, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayTextMessage
        bool showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs = -1)
        {
            if (textMessage.isEmpty()) { return false; }
            this->initInnerFrame();
            m_overlayMessages->showOverlayTextMessage(textMessage, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayVariant
        void showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs = -1)
        {
            if (m_overlayMessages->isTextMessagesActivated() && variant.canConvert<BlackMisc::Network::CTextMessage>())
            {
                this->initInnerFrame(0.75, 0.75);
                if (!this->hasMinimumSize(150, 150)) { return; }
            }
            else
            {
                this->initInnerFrame();
            }

            m_overlayMessages->showOverlayVariant(variant, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayImage
        void showOverlayImage(const BlackMisc::CPixmap &pixmap, int timeOutMs = -1)
        {
            this->initInnerFrame();
            m_overlayMessages->showOverlayImage(pixmap, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showHTMLMessage
        bool showOverlayHTMLMessage(const QString &htmlMessage, int timeOutMs = -1)
        {
            this->initMinimalFrame();
            m_overlayMessages->showHTMLMessage(htmlMessage, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc BlackGui::COverlayMessages::showHTMLMessage
        bool showOverlayHTMLMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1)
        {
            this->initMinimalFrame();
            m_overlayMessages->showHTMLMessage(message, timeOutMs);
            WIDGET::repaint();
            return true;
        }

        //! \copydoc BlackGui::COverlayMessages::showDownloadProgress
        void showDownloadProgress(int progress, qint64 current, qint64 max, const QUrl &url, int timeOutMs = -1)
        {
            this->initMinimalFrame();
            m_overlayMessages->showDownloadProgress(progress, current, max, url, timeOutMs);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayImage
        void showOverlayInlineTextMessage(Components::TextMessageTab tab)
        {
            this->initInnerFrame(0.75, 0.75);
            if (!this->hasMinimumSize(150, 150)) { return; }
            m_overlayMessages->showOverlayInlineTextMessage(tab);
            WIDGET::repaint();
        }

        //! \copydoc BlackGui::COverlayMessages::showOverlayImage
        void showOverlayInlineTextMessage(const BlackMisc::Aviation::CCallsign &callsign)
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
                BlackMisc::CLogMessage(this).info(u"Overlay widget too small (w)");
                return false;
            }

            if (h > 0 && m_overlayMessages->height() < h)
            {
                BlackMisc::CLogMessage(this).info(u"Overlay widget too small (h)");
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
            else
            {
                WIDGET::keyPressEvent(event);
            }
        }

        //! \copydoc QFrame::resizeEvent
        virtual void resizeEvent(QResizeEvent *event) override
        {
            WIDGET::resizeEvent(event);
            if (m_overlayMessages && m_overlayMessages->isVisible())
            {
                this->initInnerFrame();
            }
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
    class BLACKGUI_EXPORT COverlayMessagesFrame : public COverlayMessagesBase<QFrame>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesFrame(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    signals:
        //! Request an text message entry
        void requestTextMessageEntryTab(Components::TextMessageTab tab);

        //! Request an text message entry
        void requestTextMessageEntryCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Request a text message widget
        void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);
    };

    /*!
     * Using this class provides a QFrame with the overlay and dock widget functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesFrameEnableForDockWidgetInfoArea :
        public COverlayMessagesFrame,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesFrameEnableForDockWidgetInfoArea(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    private:
        //! Forward overlay messages
        bool isForwardingOverlayMessages() const;
    };

    /*!
     * Using this class provides a QTabWidget with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesTabWidget : public COverlayMessagesBase<QTabWidget>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTabWidget(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QWizardPage with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesWizardPage : public COverlayMessagesBase<QWizardPage>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesWizardPage(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QDockWidget with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesDockWidget : public COverlayMessagesBase<QDockWidget>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesDockWidget(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QTableView with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesTableView : public COverlayMessagesBase<QTableView>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTableView(QWidget *parent = nullptr);
    };

    /*!
     * Using this class provides a QTableView with the overlay functionality already integrated.
     */
    class BLACKGUI_EXPORT COverlayMessagesTreeView : public COverlayMessagesBase<QTreeView>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesTreeView(QWidget *parent = nullptr);
    };

} // ns

#endif // guard
