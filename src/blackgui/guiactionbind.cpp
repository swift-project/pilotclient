/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "guiactionbind.h"
#include "blackmisc/fileutils.h"

using namespace BlackMisc;
using namespace BlackCore;

namespace BlackGui
{
    CGuiActionBindHandler::CGuiActionBindHandler(QAction *action) : QObject(action)
    {
        this->setAction(action);
    }

    CGuiActionBindHandler::~CGuiActionBindHandler()
    {
        this->unbind();
    }

    CActionBindings CGuiActionBindHandler::bindMenu(QMenu *menu, const QString &path)
    {
        CActionBindings boundActions;
        if (!menu || menu->isEmpty()) { return boundActions; }
        for (QAction *action : menu->actions())
        {
            if (action->text().isEmpty()) { continue; }
            if (action->isSeparator())  { continue; }

            const QString pathNew = CGuiActionBindHandler::appendPath(path, action->text()).remove('&'); // remove E&xit key codes
            if (action->menu())
            {
                CGuiActionBindHandler::bindMenu(action->menu(), pathNew);
            }

            CGuiActionBindHandler *bindHandler = new CGuiActionBindHandler(action);
            QSharedPointer<CActionBind> actionBind(new CActionBind(pathNew, bindHandler, &CGuiActionBindHandler::boundFunction, [bindHandler]() { bindHandler->deleteLater(); }));
            bindHandler->m_index = actionBind->getIndex();
            boundActions.append(actionBind); // takes ownership
        }
        return boundActions;
    }

    void CGuiActionBindHandler::setAction(QAction *action)
    {
        this->m_action = action;

        // if the action is destroyed from somewhere else I unbind myself
        QObject::connect(action, &QAction::destroyed, [ = ]
        {
            this->unbind();
        });
    }

    void CGuiActionBindHandler::unbind()
    {
        if (!this->m_action) { return; }
        this->m_action = nullptr;
        if (CInputManager::instance())
        {
            CInputManager::instance()->unbind(this->m_index);
            m_index = -1;
        }
    }

    void CGuiActionBindHandler::boundFunction(bool enabled)
    {
        if (!enabled) { return; }
        if (!m_action) { return; }
        if (m_index < 0) { return; }
        m_action->trigger();
    }

    QString CGuiActionBindHandler::appendPath(const QString &path, const QString &name)
    {
        return CFileUtils::appendFilePaths(path, name);
    }
} // namespace
