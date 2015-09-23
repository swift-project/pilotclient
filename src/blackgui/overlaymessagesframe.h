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

#include "blackgui/overlaymessages.h"
#include <QFrame>

namespace BlackGui
{
    /*!
     * Display status messages (nested in another widget)
     */
    class COverlayMessagesFrame : public QFrame
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

        //! Show multiple messages
        void showMessages(const BlackMisc::CStatusMessageList &messages);

        //! Show single message
        void showMessage(const BlackMisc::CStatusMessage &message);

    protected:
        //! \copydoc QFrame::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

        COverlayMessages *m_statusMessages = nullptr; //!< embedded QFrame with Status messages

    private:
        //! Calculate inner frame size
        QSize innerFrameSize() const;

        //! Init the inner frame (if not yet initialized)
        void initInnerFrame();
    };

} // ns

#endif // guard
