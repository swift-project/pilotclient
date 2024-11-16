// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbairlinenameselectorcomponent.h"

#include <QCompleter>
#include <QLineEdit>
#include <QString>
#include <Qt>

#include "ui_dbairlinenameselectorcomponent.h"

#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/db/datastoreutility.h"

using namespace swift::core;
using namespace swift::gui;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::db;

namespace swift::gui::components
{
    CDbAirlineNameSelectorComponent::CDbAirlineNameSelectorComponent(QWidget *parent)
        : CDbAirlineIcaoSelectorBase(parent), ui(new Ui::CDbAirlineNameSelectorComponent)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->le_AirlineName);
        connect(ui->le_AirlineName, &QLineEdit::editingFinished, this,
                &CDbAirlineNameSelectorComponent::ps_dataChanged);
    }

    CDbAirlineNameSelectorComponent::~CDbAirlineNameSelectorComponent()
    {
        // no inline destructor, read QScopedPointer Forward Declared Pointers
    }

    bool CDbAirlineNameSelectorComponent::setAirlineIcao(const CAirlineIcaoCode &icao)
    {
        if (!CDbAirlineIcaoSelectorBase::setAirlineIcao(icao)) { return false; }
        const QString name(icao.getName());
        ui->le_AirlineName->setText(name);
        return true;
    }

    void CDbAirlineNameSelectorComponent::clear() { ui->le_AirlineName->clear(); }

    void CDbAirlineNameSelectorComponent::setReadOnly(bool readOnly) { ui->le_AirlineName->setReadOnly(readOnly); }

    QCompleter *CDbAirlineNameSelectorComponent::createCompleter()
    {
        QCompleter *c =
            new QCompleter(sGui->getWebDataServices()->getAirlineIcaoCodes().toNameCompleterStrings(), this);
        c->setCaseSensitivity(Qt::CaseInsensitive);
        c->setCompletionMode(QCompleter::PopupCompletion);
        c->setMaxVisibleItems(10);
        const int w5chars = c->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
        c->popup()->setMinimumWidth(w5chars * 10);
        ui->le_AirlineName->setCompleter(c);
        return c;
    }

    void CDbAirlineNameSelectorComponent::ps_dataChanged()
    {
        if (!sGui) { return; }
        QString s(ui->le_AirlineName->text());
        if (s.isEmpty()) { return; }
        int dbKey = CDatastoreUtility::extractIntegerKey(s);
        if (dbKey >= 0)
        {
            CAirlineIcaoCode icao(sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(dbKey));
            this->setAirlineIcao(icao);
        }
    }
} // namespace swift::gui::components
