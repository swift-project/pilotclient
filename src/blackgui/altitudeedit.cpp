/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "altitudeedit.h"
#include <QRegularExpressionValidator>
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    CAltitudeEdit::CAltitudeEdit(QWidget *parent) : QLineEdit(parent)
    {
        this->setToolTip("Altitude e.g. " + CAltitude::fpAltitudeInfo("<br>"));
        this->setPlaceholderText(CAltitude::fpAltitudeExamples());
        this->setValidator(new QRegularExpressionValidator(CAltitude::fpAltitudeRegExp(), this));
    }

    CAltitude CAltitudeEdit::getAltitude() const
    {
        CAltitude a;
        a.parseFromFpAltitudeString(this->text());
        return a;
    }

    void CAltitudeEdit::setAltitude(const CAltitude &altitude)
    {
        this->setText(altitude.asFpICAOAltitudeString());
    }

    bool CAltitudeEdit::isValid(CStatusMessageList *msgs) const
    {
        CAltitude a;
        return a.parseFromFpAltitudeString(this->text(), msgs);
    }
} // ns
