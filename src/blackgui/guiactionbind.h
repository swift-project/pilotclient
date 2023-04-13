/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIACTIONBIND_H
#define BLACKGUI_GUIACTIONBIND_H

#include "blackgui/blackguiexport.h"
#include "blackcore/actionbind.h"

#include <QMenu>
#include <QList>
#include <QPushButton>
#include <QToolButton>
#include <QScopedPointer>

namespace BlackGui
{
    //! QObject derived handler to be registered with BlackCore::CActionBind
    class BLACKGUI_EXPORT CGuiActionBindHandler : public QObject
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~CGuiActionBindHandler();

        //! Bound function for BlackCore::CActionBind
        void boundFunction(bool enabled);

        //! Bind whole menu
        //! \remark keep BlackCore::CActionBindings as long you want to keep this binding alive
        static BlackCore::CActionBindings bindMenu(QMenu *menu, const QString &path = {});

        //! Bind button, with relative name
        //! \remark keep BlackCore::CActionBinding as long you want to keep this binding alive
        static BlackCore::CActionBinding bindButton(QAbstractButton *button, const QString &path, bool absoluteName);

        //! Corresponding BlackCore::CActionBind died, so delete CGuiActionBindHandler
        static void actionBindWasDestroyed(CGuiActionBindHandler *bindHandler);

        //! Path
        static const QString &pathSwiftPilotClient();

        //! Path
        static const QString &pathSwiftCore();

    private:
        //! Constructor for QAction
        CGuiActionBindHandler(QAction *action);

        //! Constructor for QPushButton
        CGuiActionBindHandler(QAbstractButton *button);

        //! Corresponding action destroyed
        void destroyed();

        //! Set the action
        void connectDestroy(QObject *action);

        //! Unbind this action
        void unbind();

        //! Reset
        void reset();

        //! Target available?
        bool hasTarget() const;

        //! Append path for action
        static QString appendPath(const QString &path, const QString &name);

        int m_index = -1;
        QAction *m_action = nullptr;
        QAbstractButton *m_button = nullptr;
    };
} // namespace

#endif // guard
