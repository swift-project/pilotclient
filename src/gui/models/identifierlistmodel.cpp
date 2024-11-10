// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "gui/models/identifierlistmodel.h"
#include "misc/timestampbased.h"

#include <QtGlobal>

using namespace swift::misc;

namespace swift::gui::models
{
    CIdentifierListModel::CIdentifierListModel(QObject *parent) : CListModelBase("ModelIdentifierList", parent)
    {
        this->m_columns.addColumn(CColumn::standardString("name", CIdentifier::IndexName));
        this->m_columns.addColumn(CColumn::standardString("machine", CIdentifier::IndexMachineName));
        this->m_columns.addColumn(CColumn::standardString("process", CIdentifier::IndexProcessName));
        this->m_columns.addColumn(CColumn::standardString("p.id", "process id", CIdentifier::IndexProcessId));
        this->m_columns.addColumn(CColumn("lcl m.", "local machine", CIdentifier::IndexIsFromLocalMachine, new CBoolIconFormatter("local", "remote")));
        this->m_columns.addColumn(CColumn("same p.", "same process", CIdentifier::IndexIsFromSameProcess, new CBoolIconFormatter("same process", "other process")));
        this->m_columns.addColumn(CColumn::standardString("m.id", "machine id", CIdentifier::IndexMachineIdBase64));

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "name");
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "machine");
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "process");
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "p.id");
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "m.id");
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "lcl m.");
        (void)QT_TRANSLATE_NOOP("ModelIdentifierList", "same p.");
    }
} // namespace
