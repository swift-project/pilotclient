// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ui_dbairlineicaoselectorcomponent.h"
#include "core/webdataservices.h"
#include "gui/components/dbairlineicaoselectorcomponent.h"
#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "misc/db/datastoreutility.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/stringutils.h"

#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <Qt>

using namespace swift::core;
using namespace swift::gui;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::network;
using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CDbAirlineIcaoSelectorComponent::CDbAirlineIcaoSelectorComponent(QWidget *parent) : CDbAirlineIcaoSelectorBase(parent),
                                                                                        ui(new Ui::CDbAirlineIcaoSelectorComponent)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->le_Airline);
        ui->le_Airline->setValidator(new CUpperCaseValidator(this));
        connect(ui->le_Airline, &QLineEdit::editingFinished, this, &CDbAirlineIcaoSelectorComponent::onDataChanged);
    }

    CDbAirlineIcaoSelectorComponent::~CDbAirlineIcaoSelectorComponent()
    {}

    void CDbAirlineIcaoSelectorComponent::setReadOnly(bool readOnly)
    {
        ui->le_Airline->setReadOnly(readOnly);
    }

    bool CDbAirlineIcaoSelectorComponent::setAirlineIcao(const CAirlineIcaoCode &icao)
    {
        const bool changed = CDbAirlineIcaoSelectorBase::setAirlineIcao(icao);

        // Always update GUI regardless of changed because of formattimg
        const QString icaoStr(m_display == DisplayVDesignatorAndId ? this->m_currentIcao.getVDesignatorDbKey() : this->m_currentIcao.getCombinedStringWithKey());
        ui->le_Airline->setText(icaoStr);
        ui->lbl_Description->setText(this->m_currentIcao.getName());
        return changed;
    }

    void CDbAirlineIcaoSelectorComponent::displayWithIcaoDescription(bool description)
    {
        ui->lbl_Description->setVisible(description);
    }

    void CDbAirlineIcaoSelectorComponent::clear()
    {
        ui->le_Airline->clear();
        ui->lbl_Description->clear();
    }

    QString CDbAirlineIcaoSelectorComponent::getRawDesignator() const
    {
        return stripDesignatorFromCompleterString(ui->le_Airline->text());
    }

    const QStringList &CDbAirlineIcaoSelectorComponent::completerStrings()
    {
        // done for performance reasons
        // init only once, future instance can share thte list
        // only to be called when data are read!
        static const QStringList cs(sGui->getWebDataServices()->getAirlineIcaoCodes().toIcaoDesignatorCompleterStrings(true, true));
        return cs;
    }

    QCompleter *CDbAirlineIcaoSelectorComponent::createCompleter()
    {
        QCompleter *c = new QCompleter(completerStrings(), this);
        c->setCaseSensitivity(Qt::CaseInsensitive);
        c->setCompletionMode(QCompleter::PopupCompletion);
        c->setMaxVisibleItems(10);
        const int w5chars = c->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
        c->popup()->setMinimumWidth(w5chars * 10);
        ui->le_Airline->setCompleter(c);
        return c;
    }

    void CDbAirlineIcaoSelectorComponent::onDataChanged()
    {
        if (!sGui) { return; }
        QString s(ui->le_Airline->text());
        if (s.isEmpty()) { return; }
        CAirlineIcaoCode icao;
        int dbKey = CDatastoreUtility::extractIntegerKey(s);
        if (dbKey >= 0)
        {
            icao = sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(dbKey);
        }
        else
        {
            const QString designator = this->getRawDesignator();
            icao = sGui->getWebDataServices()->smartAirlineIcaoSelector(CAirlineIcaoCode(designator));
        }
        this->setAirlineIcao(icao);
    }
} // ns
