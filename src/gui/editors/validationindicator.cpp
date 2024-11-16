// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/editors/validationindicator.h"

#include <QLabel>
#include <QtGlobal>

#include "ui_validationindicator.h"

#include "gui/stylesheetutility.h"
#include "misc/icon.h"
#include "misc/icons.h"
#include "misc/statusmessage.h"

using namespace swift::misc;

namespace swift::gui::editors
{
    CValidationIndicator::CValidationIndicator(QWidget *parent) : QFrame(parent),
                                                                  ui(new Ui::CValidationIndicator)
    {
        ui->setupUi(this);
        this->clear();
        this->setFrameStyle(QFrame::StyledPanel);
        this->setFrameShadow(QFrame::Raised);
        this->setAutoFillBackground(true);
        m_originalStyleSheet = this->styleSheet();
        connect(&m_resetTimer, &QTimer::timeout, this, &CValidationIndicator::clear);
        m_resetTimer.setObjectName(this->objectName().append(":").append("resetTimer"));
        m_resetTimer.start(ResetInterval);
    }

    CValidationIndicator::~CValidationIndicator()
    {}

    void CValidationIndicator::passed()
    {
        this->show();
        this->setBackgroundColor("green");
        ui->lbl_Icon->setPixmap(CIcons::tick16());
    }

    void CValidationIndicator::failed()
    {
        this->show();
        this->setBackgroundColor("red");
        ui->lbl_Icon->setPixmap(CStatusMessage::convertToIcon(CStatusMessage::SeverityError));
    }

    void CValidationIndicator::warnings()
    {
        this->show();
        this->setBackgroundColor("yellow");
        ui->lbl_Icon->setPixmap(CStatusMessage::convertToIcon(CStatusMessage::SeverityWarning));
    }

    void CValidationIndicator::clear()
    {
        this->setBackgroundColor("");
        ui->lbl_Icon->clear();
        this->hide();
    }

    void CValidationIndicator::setState(bool ok)
    {
        if (ok)
        {
            passed();
        }
        else
        {
            failed();
        }
    }

    void CValidationIndicator::setState(const swift::misc::CStatusMessageList &msgs)
    {
        if (msgs.hasErrorMessages())
        {
            this->failed();
        }
        else if (msgs.hasWarningMessages())
        {
            this->warnings();
        }
        else
        {
            this->passed();
        }
    }

    void CValidationIndicator::paintEvent(QPaintEvent *paintEvent)
    {
        CStyleSheetUtility::useStyleSheetInDerivedWidget(this);
        Q_UNUSED(paintEvent);
    }

    void CValidationIndicator::setBackgroundColor(const QString &colorName)
    {
        if (colorName.isEmpty())
        {
            this->setStyleSheet(m_originalStyleSheet);
        }
        else
        {
            // I have to clean up any potential background image derived from style sheet
            const QString s("background-color: %1; background-image: url(:/own/icons/own/transparent1px.png);");
            this->setStyleSheet(s.arg(colorName));
            m_resetTimer.start(ResetInterval); // restart
        }
    }

} // namespace swift::gui::editors
