/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "actionbind.h"

namespace BlackCore
{
    CActionBind::~CActionBind()
    {
        unbind();
        if (m_deleteCallback)
        {
            m_deleteCallback();
        }
    }

    void CActionBind::unbind()
    {
        if (m_index < 0) { return; }
        auto inputManger = CInputManager::instance();
        inputManger->unbind(m_index);
        m_index = -1;
    }

    QString CActionBind::normalizeAction(const QString &action)
    {
        QString n = action.trimmed();
        if (!n.startsWith('/')) { return n.insert(0, QChar('/')); }
        return n;
    }
}
