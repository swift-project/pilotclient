// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ELIDEDPUSHBUTTON_H
#define SWIFT_GUI_ELIDEDPUSHBUTTON_H

#include <QPushButton>

namespace swift::gui
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
