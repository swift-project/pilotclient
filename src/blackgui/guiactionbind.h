// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_GUIACTIONBIND_H
#define BLACKGUI_GUIACTIONBIND_H

#include "blackgui/blackguiexport.h"
#include "core/actionbind.h"

#include <QMenu>
#include <QList>
#include <QPushButton>
#include <QToolButton>
#include <QScopedPointer>

namespace BlackGui
{
    //! QObject derived handler to be registered with swift::core::CActionBind
    class BLACKGUI_EXPORT CGuiActionBindHandler : public QObject
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~CGuiActionBindHandler();

        //! Bound function for swift::core::CActionBind
        void boundFunction(bool enabled);

        //! Bind whole menu
        //! \remark keep swift::core::CActionBindings as long you want to keep this binding alive
        static swift::core::CActionBindings bindMenu(QMenu *menu, const QString &path = {});

        //! Bind button, with relative name
        //! \remark keep swift::core::CActionBinding as long you want to keep this binding alive
        static swift::core::CActionBinding bindButton(QAbstractButton *button, const QString &path, bool absoluteName);

        //! Corresponding swift::core::CActionBind died, so delete CGuiActionBindHandler
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
