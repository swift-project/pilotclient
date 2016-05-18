/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

//! \file

#ifndef BLACKGUI_SHORTCUT_H
#define BLACKGUI_SHORTCUT_H

#include "blackgui/blackguiexport.h"

class QKeySequence;

namespace BlackGui
{
    /*!
     * Shortcut class to avoid overlapping shortcut defintions
     */
    class BLACKGUI_EXPORT CShortcut
    {
    public:
        //! No constructor
        CShortcut() = delete;

        //! For stashing models
        static const QKeySequence &keyStash();

        //! For deselecting all
        static const QKeySequence &keyClearSelection();

        //! For selecting all
        static const QKeySequence &keySelectAll();

        //! Display filter
        static const QKeySequence &keyDisplayFilter();

        //! Save depending on context
        static const QKeySequence &keySave();

        //! Save in views
        static const QKeySequence &keySaveViews();

        //! Delete, e.g. selected rows
        static const QKeySequence &keyDelete();

        //! Escape
        static const QKeySequence &keyEscape();
    };
} // ns
#endif // guard
