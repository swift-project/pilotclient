/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessageview.h"
#include <QHeaderView>
#include <QMenu>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CStatusMessageView::CStatusMessageView(QWidget *parent) : CViewBase(parent)
        {
            m_withMenuItemClear = true;
            this->standardInit(new CStatusMessageListModel(this));
            connect(this, &CStatusMessageView::clicked, this, &CStatusMessageView::ps_messageSelected);
        }

        void CStatusMessageView::setMode(CStatusMessageListModel::Mode mode)
        {
            this->derivedModel()->setMode(mode);
        }

        void CStatusMessageView::ps_messageSelected(const QModelIndex &index)
        {
            if (!index.isValid()) { return; }
            emit messageSelected(
                this->at(index)
            );
        }

    } // namespace
} // namespace
