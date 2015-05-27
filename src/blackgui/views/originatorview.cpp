/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "originatorview.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        COriginatorView::COriginatorView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new COriginatorListModel(this));
        }
    }
} // namespace
