/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "keyboardkeyview.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CKeyboardKeyView::CKeyboardKeyView(QWidget *parent) : CViewBase(parent)
        {
            this->m_model = new CKeyboardKeyListModel(this);
            this->setModel(this->m_model); // via QTableView
            this->m_model->setSortColumnByPropertyIndex(BlackMisc::Settings::CSettingKeyboardHotkey::IndexFunctionAsString);
            if (this->m_model->hasValidSortColumn())
                this->horizontalHeader()->setSortIndicator(
                    this->m_model->getSortColumn(),
                    this->m_model->getSortOrder());
            this->setItemDelegate(new CKeyboardKeyItemDelegate(this));
        }
    }
}
