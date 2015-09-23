/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_OVERLAYMESSAGES_H
#define BLACKGUI_OVERLAYMESSAGES_H

#include "blackmisc/statusmessagelist.h"
#include <QFrame>
#include <QScopedPointer>
#include <QTimer>

namespace Ui { class COverlayMessages; }

namespace BlackGui
{
    /*!
     * Display status messages (nested in another widget)
     */
    class COverlayMessages : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessages(int w, int h, QWidget *parent);

        //! Constructor
        explicit COverlayMessages(const QString &headerText, int w, int h, QWidget *parent);

        //! Destructor
        ~COverlayMessages();

        //! Show multiple messages
        void showMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs = -1);

        //! Show single message
        void showMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1);

        //! Messages mode
        void setModeToMessages();

        //! Single Message mode
        void setModeToMessage();

        //! Set header text
        void setHeaderText(const QString &header);

    public slots:
        //! Close button clicked
        void close();

    protected:
        //! Show message
        void display(int timeOutMs = -1);

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

        //! \copydoc QFrame::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

    private:
        QScopedPointer<Ui::COverlayMessages> ui;
        QString m_header;
        QTimer m_autoCloseTimer { this };

        //! Init widget
        void init(int w, int h);

        //! Set header text
        void setHeader(const QString &header);
    };

} // ns

#endif // guard
