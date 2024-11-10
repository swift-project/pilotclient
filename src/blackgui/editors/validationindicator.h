// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITOR_VALIDATIONINDICATOR_H
#define BLACKGUI_EDITOR_VALIDATIONINDICATOR_H

#include "blackgui/blackguiexport.h"
#include "misc/statusmessagelist.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>

class QPaintEvent;
class QWidget;

namespace Ui
{
    class CValidationIndicator;
}

namespace BlackGui::Editors
{
    /*!
     * Indication state of validation
     */
    class BLACKGUI_EXPORT CValidationIndicator : public QFrame
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
        virtual void paintEvent(QPaintEvent *paintEvent) override;

    private:
        const int ResetInterval = 5000;
        QScopedPointer<Ui::CValidationIndicator> ui;

        //! Set background color
        void setBackgroundColor(const QString &colorName);

        QTimer m_resetTimer; //!< reset to neutral
        QString m_originalStyleSheet; //!< stored, to be able to reset
    };
} // ns

#endif // guard
