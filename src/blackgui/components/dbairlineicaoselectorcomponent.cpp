/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbairlineicaoselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/stringutils.h"
#include "ui_dbairlineicaoselectorcomponent.h"

#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <Qt>

using namespace BlackCore;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Components
    {
        CDbAirlineIcaoSelectorComponent::CDbAirlineIcaoSelectorComponent(QWidget *parent) :
            CDbAirlineIcaoSelectorBase(parent),
            ui(new Ui::CDbAirlineIcaoSelectorComponent)
        {
            ui->setupUi(this);
            ui->le_Airline->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_Airline, &QLineEdit::returnPressed, this, &CDbAirlineIcaoSelectorComponent::ps_dataChanged);
        }

        CDbAirlineIcaoSelectorComponent::~CDbAirlineIcaoSelectorComponent()
        {
            // no inline destructor, read QScopedPointer Forward Declared Pointers
        }

        void CDbAirlineIcaoSelectorComponent::setReadOnly(bool readOnly)
        {
            this->ui->le_Airline->setReadOnly(readOnly);
        }

        bool CDbAirlineIcaoSelectorComponent::setAirlineIcao(const CAirlineIcaoCode &icao)
        {
            if (!CDbAirlineIcaoSelectorBase::setAirlineIcao(icao)) { return false; }
            const QString icaoStr(icao.getVDesignator());
            this->ui->le_Airline->setText(icaoStr);
            ui->lbl_Description->setText(icao.getName());
            return true;
        }

        void CDbAirlineIcaoSelectorComponent::withIcaoDescription(bool description)
        {
            this->ui->lbl_Description->setVisible(description);
        }

        void CDbAirlineIcaoSelectorComponent::clear()
        {
            this->ui->le_Airline->clear();
            this->ui->lbl_Description->clear();
        }

        QString CDbAirlineIcaoSelectorComponent::getRawDesignator() const
        {
            return stripDesignatorFromCompleterString(ui->le_Airline->text());
        }

        QCompleter *CDbAirlineIcaoSelectorComponent::createCompleter()
        {
            QCompleter *c = new QCompleter(sGui->getWebDataServices()->getAirlineIcaoCodes().toIcaoDesignatorCompleterStrings(true, true), this);
            c->setCaseSensitivity(Qt::CaseInsensitive);
            c->setCompletionMode(QCompleter::PopupCompletion);
            c->setMaxVisibleItems(10);
            this->ui->le_Airline->setCompleter(c);
            return c;
        }

        void CDbAirlineIcaoSelectorComponent::ps_dataChanged()
        {
            if (!sGui) { return; }
            QString s(this->ui->le_Airline->text());
            if (s.isEmpty()) { return; }
            int dbKey = CDatastoreUtility::extractIntegerKey(s);
            if (dbKey >= 0)
            {
                CAirlineIcaoCode icao(sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(dbKey));
                this->setAirlineIcao(icao);
            }
        }
    }// class
} // ns
