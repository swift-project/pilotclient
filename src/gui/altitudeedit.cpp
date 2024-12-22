// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "altitudeedit.h"

#include <QRegularExpressionValidator>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;

namespace swift::gui
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

    void CAltitudeEdit::setAltitude(const CAltitude &altitude) { this->setText(altitude.asFpICAOAltitudeString()); }

    bool CAltitudeEdit::isValid(CStatusMessageList *msgs) const
    {
        CAltitude a;
        return a.parseFromFpAltitudeString(this->text(), msgs);
    }
} // namespace swift::gui
