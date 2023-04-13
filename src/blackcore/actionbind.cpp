/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/actionbind.h"

namespace BlackCore
{
    CActionBind::CActionBind(const QString &action, BlackMisc::CIcons::IconIndex icon, QObject *parent)
        : QObject(parent)
    {
        CActionBind::registerAction(action, icon);
    }

    QString CActionBind::registerAction(const QString &action, BlackMisc::CIcons::IconIndex icon)
    {
        if (!sApp || sApp->isShuttingDown()) { return {}; }
        if (sApp->getApplicationInfo().isUnitTest()) { return {}; }

        const QString a = CActionBind::normalizeAction(action);
        Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
        sApp->getInputManager()->registerAction(a, icon);
        return a;
    }

    CActionBind::~CActionBind()
    {
        // void
    }

    void CActionBind::unbind()
    {
        if (m_index < 0) { return; }
        if (sApp->getApplicationInfo().isUnitTest()) { return; }

        Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
        sApp->getInputManager()->unbind(m_index);
        m_index = -1;
    }

    void CActionBind::shutdown()
    {
        unbind();
        if (m_deleteCallback)
        {
            m_deleteCallback();
        }
    }

    QString CActionBind::normalizeAction(const QString &action)
    {
        QString n = action.trimmed();
        if (!n.startsWith('/')) { n.insert(0, QChar('/')); }
        if (n.endsWith('/')) { n.remove(n.length() - 1, 1); }
        return n;
    }
}
