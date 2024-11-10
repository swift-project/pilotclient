// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/filterdialog.h"
#include "gui/guiapplication.h"
#include "gui/stylesheetutility.h"

#include <QString>
#include <Qt>

namespace swift::gui::filters
{
    CFilterDialog::CFilterDialog(QWidget *parent) : QDialog(parent, Qt::Tool)
    {
        this->setWindowTitle("Filter dialog");
        this->onStyleSheetChanged();
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CFilterDialog::onStyleSheetChanged, Qt::QueuedConnection);
    }

    CFilterDialog::~CFilterDialog()
    {}

    void CFilterDialog::onStyleSheetChanged()
    {
        const QString qss = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameFilterDialog());
        this->setStyleSheet(qss);
    }

} // namespace
