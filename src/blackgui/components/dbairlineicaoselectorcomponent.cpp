/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_dbairlineicaoselectorcomponent.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/dbairlineicaoselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/stringutils.h"

#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <Qt>

using namespace BlackCore;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackGui::Components
{
    CDbAirlineIcaoSelectorComponent::CDbAirlineIcaoSelectorComponent(QWidget *parent) :
        CDbAirlineIcaoSelectorBase(parent),
        ui(new Ui::CDbAirlineIcaoSelectorComponent)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->le_Airline);
        ui->le_Airline->setValidator(new CUpperCaseValidator(this));
        connect(ui->le_Airline, &QLineEdit::editingFinished, this, &CDbAirlineIcaoSelectorComponent::onDataChanged);
    }

    CDbAirlineIcaoSelectorComponent::~CDbAirlineIcaoSelectorComponent()
    { }

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
