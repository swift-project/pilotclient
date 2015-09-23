/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "filterdialog.h"
#include "blackgui/stylesheetutility.h"

namespace BlackGui
{
    namespace Filters
    {
        CFilterDialog::CFilterDialog(QWidget *parent) : QDialog(parent, Qt::Tool)
        {
            this->setWindowTitle("Filter dialog");
            ps_onStyleSheetChanged();
            connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CFilterDialog::ps_onStyleSheetChanged);
        }

        CFilterDialog::~CFilterDialog()
        { }

        void CFilterDialog::ps_onStyleSheetChanged()
        {
            const QString qss = CStyleSheetUtility::instance().style(CStyleSheetUtility::fileNameFilterDialog());
            this->setStyleSheet(qss);
        }

    } // namespace
} // namespace


