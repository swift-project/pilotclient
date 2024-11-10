// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/filterwidget.h"
#include "gui/guiapplication.h"
#include "gui/stylesheetutility.h"

#include <QString>
#include <QtGlobal>

namespace swift::gui::filters
{
    CFilterWidget::CFilterWidget(QWidget *parent) : QFrame(parent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "sGui missing");
        this->setWindowTitle("Filter widget");
        this->onStyleSheetChanged();
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CFilterWidget::onStyleSheetChanged, Qt::QueuedConnection);
    }

    CFilterWidget::~CFilterWidget()
    {}

    void CFilterWidget::setButtonsAndCount(CFilterBarButtons *buttons)
    {
        if (buttons)
        {
            bool s = connect(buttons, &CFilterBarButtons::buttonClicked, this, &CFilterWidget::onFilterButtonClicked);
            Q_ASSERT_X(s, Q_FUNC_INFO, "filter button connect");
            Q_UNUSED(s);
        }
    }

    int CFilterWidget::convertDbId(const QString &candidate)
    {
        const QString s = candidate.trimmed();
        if (s.isEmpty()) { return -1; }
        bool ok = false;
        const int id = s.toInt(&ok);
        return ok && id >= 0 ? id : -1;
    }

    void CFilterWidget::triggerFilter()
    {
        this->onFilterButtonClicked(CFilterBarButtons::Filter);
    }

    void CFilterWidget::onStyleSheetChanged()
    {
        const QString qss = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameFilterDialog());
        this->setStyleSheet(qss);
    }

    void CFilterWidget::onFilterButtonClicked(CFilterBarButtons::FilterButton filterButton)
    {
        switch (filterButton)
        {
        case CFilterBarButtons::Filter:
            emit this->changeFilter(true);
            break;
        case CFilterBarButtons::RemoveFilter:
            emit this->changeFilter(false);
            emit this->rejectDialog();
            break;
        case CFilterBarButtons::ClearForm:
            this->clearForm();
            emit this->changeFilter(false);
            break;
        }
    }
} // namespace
