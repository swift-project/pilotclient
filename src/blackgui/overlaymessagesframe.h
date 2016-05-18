/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_OVERLAYMESSAGES_FRAME_H
#define BLACKGUI_OVERLAYMESSAGES_FRAME_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QMessageBox>
#include <QObject>
#include <QSize>
#include <QString>
#include <functional>

class QKeyEvent;
class QPaintEvent;
class QWidget;

namespace BlackMisc { namespace Network { class CTextMessage; } }

namespace BlackGui
{
    class COverlayMessages;

    /*!
     * Display status messages (nested in another widget)
     */
    class BLACKGUI_EXPORT COverlayMessagesFrame : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessagesFrame(QWidget *parent = nullptr);

        //! Destructor
        ~COverlayMessagesFrame();

        //! Show the inner frame
        void showStatusMessagesFrame();

        //! Hide the inner frame
        void hideStatusMessagesFrame();

        //! \copydoc COverlayMessages::showOverlayMessagesWithConfirmation
        void showOverlayMessagesWithConfirmation(
            const BlackMisc::CStatusMessageList &messages,
            const QString                       &confirmationMessage,
            std::function<void()>                okLambda,
            int                                  defaultButton = QMessageBox::Cancel,
            int                                  timeOutMs = -1
        );

    public slots:
        //! \copydoc COverlayMessages::showOverlayMessages
        void showOverlayMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs = -1);

        //! \copydoc COverlayMessages::showOverlayMessage
        void showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1);

        //! \copydoc COverlayMessages::showOverlayTextMessage
        void showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs = -1);

        //! \copydoc COverlayMessages::showOverlayVariant
        void showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs = -1);

        //! \copydoc COverlayMessages::showOverlayImage
        void showOverlayImage(const BlackMisc::CPixmap &pixmap, int timeOutMs = -1);

    protected:
        //! \copydoc QFrame::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

        COverlayMessages *m_overlayMessages = nullptr; //!< embedded QFrame with Status messages

    private:
        //! Calculate inner frame size
        QSize innerFrameSize() const;

        //! Init the inner frame (if not yet initialized)
        void initInnerFrame();
    };
} // ns

#endif // guard
