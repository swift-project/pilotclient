/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/filters/filterwidget.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"

#include <QString>
#include <QtGlobal>

namespace BlackGui
{
    namespace Filters
    {
        CFilterWidget::CFilterWidget(QWidget *parent) : QFrame(parent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "sGui missing");
            this->setWindowTitle("Filter widget");
            ps_onStyleSheetChanged();
            connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CFilterWidget::ps_onStyleSheetChanged, Qt::QueuedConnection);
        }

        CFilterWidget::~CFilterWidget()
        { }

        void CFilterWidget::setButtonsAndCount(CFilterBarButtons *buttons)
        {
            if (buttons)
            {
                bool s = connect(buttons, &CFilterBarButtons::buttonClicked, this, &CFilterWidget::ps_filterButtonClicked);
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
            this->ps_filterButtonClicked(CFilterBarButtons::Filter);
        }

        void CFilterWidget::ps_onStyleSheetChanged()
        {
            const QString qss = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameFilterDialog());
            this->setStyleSheet(qss);
        }

        void CFilterWidget::ps_filterButtonClicked(CFilterBarButtons::FilterButton filterButton)
        {
            switch (filterButton)
            {
            case CFilterBarButtons::Filter:
                emit changeFilter(true);
                break;
            case CFilterBarButtons::RemoveFilter:
                emit changeFilter(false);
                emit rejectDialog();
                break;
            case CFilterBarButtons::ClearForm:
                clearForm();
                emit changeFilter(false);
                break;
            }
        }
    } // namespace
} // namespace
