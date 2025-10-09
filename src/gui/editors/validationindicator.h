// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_EDITOR_VALIDATIONINDICATOR_H
#define SWIFT_GUI_EDITOR_VALIDATIONINDICATOR_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>

#include "gui/swiftguiexport.h"
#include "misc/statusmessagelist.h"

class QPaintEvent;
class QWidget;

namespace Ui
{
    class CValidationIndicator;
}

namespace swift::gui::editors
{
    /*!
     * Indication state of validation
     */
    class SWIFT_GUI_EXPORT CValidationIndicator : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CValidationIndicator(QWidget *parent = nullptr);

        //! Destructor
        ~CValidationIndicator() override;

        //! Validation passed
        void passed();

        //! Passed with warnings
        void warnings();

        //! Validation failed
        void failed();

        //! Ok or failed validation?
        void setState(bool ok);

        //! Messages from status messages
        void setState(const swift::misc::CStatusMessageList &msgs);

    public slots:
        //! Cleared state
        void clear();

    protected:
        //! \copydoc QFrame::paintEvent
        void paintEvent(QPaintEvent *paintEvent) override;

    private:
        const int ResetInterval = 5000;
        QScopedPointer<Ui::CValidationIndicator> ui;

        //! Set background color
        void setBackgroundColor(const QString &colorName);

        QTimer m_resetTimer; //!< reset to neutral
        QString m_originalStyleSheet; //!< stored, to be able to reset
    };
} // namespace swift::gui::editors

#endif // SWIFT_GUI_EDITOR_VALIDATIONINDICATOR_H
