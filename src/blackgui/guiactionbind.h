/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIACTIONBIND_H
#define BLACKGUI_GUIACTIONBIND_H

#include "blackgui/blackguiexport.h"
#include "blackcore/actionbind.h"

#include <QMenu>
#include <QList>
#include <QScopedPointer>

namespace BlackGui
{
    //! QObject derived handler to be registered with BlackCore::CActionBind
    class BLACKGUI_EXPORT CGuiActionBindHandler : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CGuiActionBindHandler(QAction *action);

        //! Destructor
        virtual ~CGuiActionBindHandler();

        //! Bound function for BlackCore::CActionBind
        void boundFunction(bool enabled);

        //! Bind whole menu
        static BlackCore::CActionBindings bindMenu(QMenu *menu, const QString &path = {});

    private:
        //! Corresponding action destroyed
        void destroyed();

        //! Set the action
        void setAction(QAction *action);

        //! Unbind this action
        void unbind();

        //! Append path for action
        static QString appendPath(const QString &path, const QString &name);

        int m_index = -1;
        QAction *m_action = nullptr;
    };
} // namespace

#endif // guard
