/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/filters/filterdialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"

#include <QString>
#include <Qt>

namespace BlackGui::Filters
{
    CFilterDialog::CFilterDialog(QWidget *parent) : QDialog(parent, Qt::Tool)
    {
        this->setWindowTitle("Filter dialog");
        this->onStyleSheetChanged();
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CFilterDialog::onStyleSheetChanged, Qt::QueuedConnection);
    }

    CFilterDialog::~CFilterDialog()
    { }

    void CFilterDialog::onStyleSheetChanged()
    {
        const QString qss = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameFilterDialog());
        this->setStyleSheet(qss);
    }

} // namespace
