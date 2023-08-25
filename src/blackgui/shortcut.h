// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
// Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com

//! \file

#ifndef BLACKGUI_SHORTCUT_H
#define BLACKGUI_SHORTCUT_H

#include "blackgui/blackguiexport.h"

class QKeySequence;

namespace BlackGui
{
    /*!
     * Shortcut definitions to avoid duplicated shortcut defintions
     */
    class BLACKGUI_EXPORT CShortcut
    {
    public:
        //! No constructor
        CShortcut() = delete;

        //! For stashing models
        static const QKeySequence &keyStash();

        //! Add to model set
        static const QKeySequence &keyAddToModelSet();

        //! For deselecting all
        static const QKeySequence &keyClearSelection();

        //! For selecting all
        static const QKeySequence &keySelectAll();

        //! Display filter
        static const QKeySequence &keyDisplayFilter();

        //! Resize view
        static const QKeySequence &keyResizeView();

        //! Save depending on context
        static const QKeySequence &keySave();

        //! Save in views
        static const QKeySequence &keySaveViews();

        //! Delete, e.g. selected rows
        static const QKeySequence &keyDelete();

        //! Escape
        static const QKeySequence &keyEscape();

        //! Copy
        static const QKeySequence &keyCopy();

        //! Font plus
        static const QKeySequence &keyFontPlus();

        //! Font minus
        static const QKeySequence &keyFontMinus();

        //! Font reset
        static const QKeySequence &keyFontReset();

        //! As string for menus etc. Looks like "(CTRL + R)"
        static QString toParenthesisString(const QKeySequence &sequence);
    };
} // ns
#endif // guard
