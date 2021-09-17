/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ELIDEDPUSHBUTTON_H
#define BLACKGUI_ELIDEDPUSHBUTTON_H

#include <QPushButton>

namespace BlackGui
{
    /*!
     * Push button with elided text
     * \remark based on http://doc.qt.io/qt-5/qtwidgets-widgets-elidedlabel-example.html
     */
    class CElidedPushButton : public QPushButton
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CElidedPushButton(QWidget *parent = nullptr);

        //! Constructor
        explicit CElidedPushButton(const QString &text, QWidget *parent = nullptr);

        //! Text
        void setText(const QString &text);

        //! Text
        const QString &text() const { return m_content; }

        //! Elided?
        bool isElided() const { return m_elided; }

    protected:
        //! \copydoc QPushButton::paintEvent
        void paintEvent(QPaintEvent *event) override;

    private:
        bool m_elided = false;
        QString m_content;
    };
} // ns

#endif // guard
