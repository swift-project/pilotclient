// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/transpondercodespinbox.h"

#include <QChar>

namespace swift::gui::components
{
    CTransponderCodeSpinBox::CTransponderCodeSpinBox(QWidget *parent) : QSpinBox(parent) {}

    QString CTransponderCodeSpinBox::textFromValue(int value) const
    {
        /* 4 - number of digits, 10 - base of number, '0' - pad character*/
        return QStringLiteral("%1").arg(value, 4, 10, QChar('0'));
    }
} // namespace swift::gui::components
