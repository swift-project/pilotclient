/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/transpondercodespinbox.h"
#include <QChar>

namespace BlackGui::Components
{
    CTransponderCodeSpinBox::CTransponderCodeSpinBox(QWidget *parent) :
        QSpinBox(parent)
    { }

    QString CTransponderCodeSpinBox::textFromValue(int value) const
    {
        /* 4 - number of digits, 10 - base of number, '0' - pad character*/
        return QStringLiteral("%1").arg(value, 4 , 10, QChar('0'));
    }
} // ns
