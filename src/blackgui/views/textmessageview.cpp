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

namespace BlackGui
{
    namespace Views
    {
        CTextMessageView::CTextMessageView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CTextMessageListModel(CTextMessageListModel::FromTo, this));
            this->m_menus |= MenuClear;
        }

        void CTextMessageView::setTextMessageMode(CTextMessageListModel::TextMessageMode mode)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setTextMessageMode(mode);
            this->setSortIndicator();
        }

    }
} // namespace
