/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "guiactionbind.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/imageutils.h"

using namespace BlackMisc;
using namespace BlackCore;

namespace BlackGui
{
    CGuiActionBindHandler::CGuiActionBindHandler(QAction *action) : QObject(action), m_action(action)
    {
        this->connectDestroy(action);
        connect(sApp, &CApplication::aboutToShutdown, this, &CGuiActionBindHandler::unbind);
    }

    CGuiActionBindHandler::CGuiActionBindHandler(QAbstractButton *button) : QObject(button), m_button(button)
    {
        this->connectDestroy(button);
        connect(sApp, &CApplication::aboutToShutdown, this, &CGuiActionBindHandler::unbind);
    }

    CGuiActionBindHandler::~CGuiActionBindHandler()
    {
    }

    CActionBindings CGuiActionBindHandler::bindMenu(QMenu *menu, const QString &path)
    {
        Q_ASSERT(menu);
        CActionBindings boundActions;
        if (!menu || menu->isEmpty()) { return boundActions; }
        for (QAction *action : menu->actions())
        {
            if (action->text().isEmpty()) { continue; }
            if (action->isSeparator()) { continue; }

            const QString pathNew = CGuiActionBindHandler::appendPath(path, action->text()).remove('&'); // remove E&xit key codes
            if (action->menu())
            {
                CGuiActionBindHandler::bindMenu(action->menu(), pathNew);
            }

            const bool hasIcon = !action->icon().isNull();
            CGuiActionBindHandler *bindHandler = new CGuiActionBindHandler(action);
            // MS 2019-10-08 [AFV integration] CActionBind constructor needs an icon index, not a QPixmap
            // CActionBinding actionBinding(CActionBinding::create(pathNew, hasIcon ? action->icon().pixmap(CIcons::empty16().size()) : CIcons::empty16(), bindHandler, &CGuiActionBindHandler::boundFunction, [bindHandler]() { CGuiActionBindHandler::actionBindWasDestroyed(bindHandler); }));
            CActionBinding actionBinding(CActionBinding::create(pathNew, CIcons::StandardIconEmpty16, bindHandler, &CGuiActionBindHandler::boundFunction, [bindHandler]() { CGuiActionBindHandler::actionBindWasDestroyed(bindHandler); }));
            Q_UNUSED(hasIcon)
            bindHandler->m_index = actionBinding->getIndex();
            boundActions.append(actionBinding); // takes ownership
        }
        return boundActions;
    }

    CActionBinding CGuiActionBindHandler::bindButton(QAbstractButton *button, const QString &path, bool absoluteName)
    {
        Q_ASSERT(button);
        const QString pathNew = absoluteName ?
                                    path :
                                    CGuiActionBindHandler::appendPath(path, button->text()).remove('&'); // remove E&xit key codes
        CGuiActionBindHandler *bindHandler = new CGuiActionBindHandler(button);
        const bool hasIcon = !button->icon().isNull();
        // MS 2019-10-08 [AFV integration] CActionBind constructor needs an icon index, not a QPixmap
        // CActionBinding actionBinding(CActionBinding::create(pathNew, hasIcon ? button->icon().pixmap(CIcons::empty16().size()) : CIcons::empty16(), bindHandler, &CGuiActionBindHandler::boundFunction, [bindHandler]() { CGuiActionBindHandler::actionBindWasDestroyed(bindHandler); }));
        CActionBinding actionBinding(CActionBinding::create(pathNew, CIcons::StandardIconEmpty16, bindHandler, &CGuiActionBindHandler::boundFunction, [bindHandler]() { CGuiActionBindHandler::actionBindWasDestroyed(bindHandler); }));
        Q_UNUSED(hasIcon)
        bindHandler->m_index = actionBinding->getIndex();
        return actionBinding;
    }

    void CGuiActionBindHandler::actionBindWasDestroyed(CGuiActionBindHandler *bindHandler)
    {
        if (!bindHandler) { return; }
        bindHandler->reset();
        // do not delete, as it might be still referenced somewhere
    }

    void CGuiActionBindHandler::connectDestroy(QObject *object)
    {
        // if the action is destroyed from somewhere else I unbind myself
        QObject::connect(object, &QObject::destroyed, [=] {
            this->unbind();
        });
    }

    void CGuiActionBindHandler::unbind()
    {
        if (this->hasTarget())
        {
            Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
            sApp->getInputManager()->unbind(m_index);
        }
        this->reset();
    }

    void CGuiActionBindHandler::reset()
    {
        m_index = -1;
        m_button = nullptr;
        m_action = nullptr;
    }

    bool CGuiActionBindHandler::hasTarget() const
    {
        return (m_button || m_action) && m_index >= 0;
    }

    void CGuiActionBindHandler::boundFunction(bool enabled)
    {
        if (!enabled || !this->hasTarget()) { return; }
        if (m_action)
        {
            m_action->trigger();
        }
        else if (m_button)
        {
            m_button->click();
        }
    }

    QString CGuiActionBindHandler::appendPath(const QString &path, const QString &name)
    {
        return CFileUtils::appendFilePaths(path, name);
    }

    const QString &CGuiActionBindHandler::pathSwiftPilotClient()
    {
        static const QString s("Pilot client UI/");
        return s;
    }

    const QString &CGuiActionBindHandler::pathSwiftCore()
    {
        static const QString s("Core UI/");
        return s;
    }
} // namespace
