/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbairlineicaoselectorcomponent.h"
#include "ui_dbairlineicaoselectorcomponent.h"
#include "blackgui/guiutility.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/datastoreutility.h"
#include <QMimeData>

using namespace BlackGui;
using namespace BlackMisc;
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

        void CDbAirlineIcaoSelectorComponent::setAirlineIcao(const CAirlineIcaoCode &icao)
        {
            CDbAirlineIcaoSelectorBase::setAirlineIcao(icao);
            const QString icaoStr(icao.getVDesignator());
            this->ui->le_Airline->setText(icaoStr);
            ui->lbl_Description->setText(icao.getName());
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

        QCompleter *CDbAirlineIcaoSelectorComponent::createCompleter()
        {
            QCompleter *c = new QCompleter(this->getAirlineIcaoCodes().toIcaoDesignatorCompleterStrings(), this);
            c->setCaseSensitivity(Qt::CaseInsensitive);
            c->setCompletionMode(QCompleter::PopupCompletion);
            c->setMaxVisibleItems(10);
            this->ui->le_Airline->setCompleter(c);
            return c;
        }

        void CDbAirlineIcaoSelectorComponent::ps_dataChanged()
        {
            if (!hasProvider()) { return; }
            QString s(this->ui->le_Airline->text());
            if (s.isEmpty()) { return; }
            int dbKey = CDatastoreUtility::extractIntegerKey(s);
            if (dbKey >= 0)
            {
                CAirlineIcaoCode icao(getAirlineIcaoCodeForDbKey(dbKey));
                this->setAirlineIcao(icao);
            }
            else
            {
                // second choice, first object by name
                // CAirlineIcaoCode icao(getAirlineDesignatorWithName(s, starting with));
                // this->setAirlineIcao(icao);
            }
        }
    }// class
} // ns
