/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/namevariantpairlistmodel.h"
#include "blackgui/views/namevariantpairview.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CNameVariantPairView::CNameVariantPairView(QWidget *parent) : CViewBase(parent)
    {
        m_forceStretchLastColumnWhenResized = true;
        this->standardInit(new CNameVariantPairModel(true, this));
    }

    void CNameVariantPairView::setIconMode(bool withIcon)
    {
        Q_ASSERT(m_model);
        m_model->setIconMode(withIcon);
    }

    bool CNameVariantPairView::addOrUpdateByName(const QString &name, const BlackMisc::CVariant &value, const CIcon &icon, bool resize, bool skipEqualValues)
    {
        Q_ASSERT(m_model);
        bool changed = m_model->addOrUpdateByName(name, value, icon, skipEqualValues);
        if (resize && changed) { this->resizeToContents(); }
        return changed;
    }

    void CNameVariantPairView::removeByName(const QString &name, bool resize)
    {
        m_model->removeByName(name);
        if (resize) { this->resizeToContents(); }
    }

    bool CNameVariantPairView::containsName(const QString &name)
    {
        return m_model->containsName(name);
    }
} // namespace
