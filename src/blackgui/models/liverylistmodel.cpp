/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "liverylistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        CLiveryListModel::CLiveryListModel(QObject *parent) :
            CListModelBase("ModelLiveryList", parent)
        {
            this->m_columns.addColumn(CColumn::standardString("code", CLivery::IndexCombinedCode));
            this->m_columns.addColumn(CColumn::standardString("description", CLivery::IndexDescription));
            this->m_columns.addColumn(CColumn::standardString("fuselage", CLivery::IndexColorFuselage));
            this->m_columns.addColumn(CColumn::standardString("tail", CLivery::IndexColorTail));
            this->m_columns.addColumn(CColumn("mil.", "military", CLivery::IndexIsMilitary, new CBoolIconFormatter("military", "civil")));

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelLiveryList", "key");
            (void)QT_TRANSLATE_NOOP("ModelLiveryList", "description");
            (void)QT_TRANSLATE_NOOP("ModelLiveryList", "fuselage");
            (void)QT_TRANSLATE_NOOP("ModelLiveryList", "tail");
            (void)QT_TRANSLATE_NOOP("ModelLiveryList", "mil.");
            (void)QT_TRANSLATE_NOOP("ModelLiveryList", "military");
        }
    } // class
} // namespace
