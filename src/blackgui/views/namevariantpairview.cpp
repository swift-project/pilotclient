/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "namevariantpairview.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CNameVariantPairView::CNameVariantPairView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CNameVariantPairModel(true, this));
        }

        void CNameVariantPairView::setIconMode(bool withIcon)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setIconMode(withIcon);
        }

        bool CNameVariantPairView::addOrUpdateByName(const QString &name, const QVariant &value, const CIcon &icon, bool resize, bool skipEqualValues)
        {
            Q_ASSERT(this->m_model);
            bool changed = this->m_model->addOrUpdateByName(name, value, icon, skipEqualValues);
            if (resize && changed) { this->resizeToContents(); }
            return changed;
        }

        void CNameVariantPairView::removeByName(const QString &name, bool resize)
        {
            this->m_model->removeByName(name);
            if (resize) { this->resizeToContents(); }
        }

        bool CNameVariantPairView::containsName(const QString &name)
        {
            return this->m_model->containsName(name);
        }
    }
}
