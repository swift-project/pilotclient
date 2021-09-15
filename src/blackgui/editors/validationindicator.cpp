/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/editors/validationindicator.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/icon.h"
#include "blackmisc/icons.h"
#include "blackmisc/statusmessage.h"
#include "ui_validationindicator.h"

#include <QLabel>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui::Editors
{
    CValidationIndicator::CValidationIndicator(QWidget *parent) :
        QFrame(parent),
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
    { }

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

    void CValidationIndicator::setState(const BlackMisc::CStatusMessageList &msgs)
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

} // ns
