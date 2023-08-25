// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
// Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com

#include "blackgui/shortcut.h"

#include <QKeySequence>
#include <Qt>

namespace BlackGui
{
    const QKeySequence &CShortcut::keyStash()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S);
        return k;
    }

    const QKeySequence &CShortcut::keyAddToModelSet()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_A);
        return k;
    }

    const QKeySequence &CShortcut::keyClearSelection()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_Space);
        return k;
    }

    const QKeySequence &CShortcut::keySelectAll()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_A);
        return k;
    }

    const QKeySequence &BlackGui::CShortcut::keyDisplayFilter()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_F);
        return k;
    }

    const QKeySequence &CShortcut::keyResizeView()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_R);
        return k;
    }

    const QKeySequence &CShortcut::keySave()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_S);
        return k;
    }

    const QKeySequence &CShortcut::keySaveViews()
    {
        // remark CTRL+S not working in views
        static const QKeySequence k(static_cast<Qt::Key>(Qt::SHIFT) + Qt::Key_S);
        return k;
    }

    const QKeySequence &CShortcut::keyDelete()
    {
        static const QKeySequence k(Qt::Key_Delete);
        return k;
    }

    const QKeySequence &CShortcut::keyEscape()
    {
        static const QKeySequence k(Qt::Key_Escape);
        return k;
    }

    const QKeySequence &CShortcut::keyCopy()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_C);
        return k;
    }

    const QKeySequence &CShortcut::keyFontPlus()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_Plus);
        return k;
    }

    const QKeySequence &CShortcut::keyFontMinus()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_Minus);
        return k;
    }

    const QKeySequence &CShortcut::keyFontReset()
    {
        static const QKeySequence k(static_cast<Qt::Key>(Qt::CTRL) + Qt::Key_0);
        return k;
    }

    QString CShortcut::toParenthesisString(const QKeySequence &sequence)
    {
        return QStringLiteral("(%1)").arg(sequence.toString());
    }
} // ns
