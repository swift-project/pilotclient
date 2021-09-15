/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/textmessagelistmodel.h"
#include "blackgui/views/textmessageview.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CTextMessageView::CTextMessageView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CTextMessageListModel(CTextMessageListModel::FromTo, this));
        m_menus |= MenuClear;
    }

    void CTextMessageView::setTextMessageMode(CTextMessageListModel::TextMessageMode mode)
    {
        Q_ASSERT(m_model);
        m_model->setTextMessageMode(mode);
        this->setSortIndicator();
    }

    bool CTextMessageView::isSortedByTimestampProperty() const
    {
        return m_model->isSortedByTimestampProperty();
    }

    bool CTextMessageView::isSortedByTimestampPropertyLatestLast() const
    {
        return m_model->isSortedByTimestampProperty() && this->getSortOrder() == Qt::AscendingOrder;
    }

} // namespace
