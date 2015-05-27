/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "originatorlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        COriginatorListModel::COriginatorListModel(QObject *parent) :
            CListModelBase("ModelOriginatorList", parent)
        {
            this->m_columns.addColumn(CColumn::standardString("name", COriginator::IndexName));
            this->m_columns.addColumn(CColumn::standardString("machine", COriginator::IndexMachineName));
            this->m_columns.addColumn(CColumn::standardString("process", COriginator::IndexProcessName));
            this->m_columns.addColumn(CColumn::standardString("p.id", "process id", COriginator::IndexProcessId));
            this->m_columns.addColumn(CColumn("time", "received", COriginator::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHms())));
            this->m_columns.addColumn(CColumn("lcl m.", "local machine", COriginator::IndexIsFromLocalMachine, new CBoolIconFormatter("local", "remote")));
            this->m_columns.addColumn(CColumn("same p.", "same process", COriginator::IndexIsFromSameProcess, new CBoolIconFormatter("same process", "other process")));
            this->m_columns.addColumn(CColumn::standardString("m.id", "machine id", COriginator::IndexMachineIdBase64));

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "name");
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "machine");
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "process");
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "p.id");
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "m.id");
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "lcl m.");
            (void)QT_TRANSLATE_NOOP("ModelOriginatorList", "same p.");
        }

    } // class
} // namespace
