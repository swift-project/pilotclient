/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbcountryselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/variant.h"
#include "ui_dbcountryselectorcomponent.h"

#include <QCompleter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMetaType>
#include <QPalette>
#include <QPixmap>
#include <Qt>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CDbCountrySelectorComponent::CDbCountrySelectorComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CDbCountrySelectorComponent)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->le_CountryIso);
        this->setAcceptDrops(true);
        this->setAcceptedMetaTypeIds({qMetaTypeId<CCountry>(), qMetaTypeId<CCountryList>()});

        connect(ui->le_CountryIso, &QLineEdit::returnPressed, this, &CDbCountrySelectorComponent::onDataChanged);
        connect(ui->le_CountryName, &QLineEdit::returnPressed, this, &CDbCountrySelectorComponent::onDataChanged);
        connect(ui->le_CountryIso, &QLineEdit::editingFinished, this, &CDbCountrySelectorComponent::onDataChanged);
        connect(ui->le_CountryName, &QLineEdit::returnPressed, this, &CDbCountrySelectorComponent::onDataChanged);

        ui->le_CountryIso->setValidator(new CUpperCaseValidator(this));
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbCountrySelectorComponent::onCountriesRead, Qt::QueuedConnection);
        this->onCountriesRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getCountriesCount());
    }

    CDbCountrySelectorComponent::~CDbCountrySelectorComponent()
    { }

    void CDbCountrySelectorComponent::setCountry(const BlackMisc::CCountry &country)
    {
        ui->le_CountryIso->setText(country.getIsoCode());
        ui->le_CountryName->setText(country.getName());
        ui->lbl_CountryIcon->setPixmap(CIcon(country.toIcon()));
        if (country != m_currentCountry)
        {
            m_currentCountry = country;
            emit countryChanged(country);
        }
    }

    void CDbCountrySelectorComponent::setCountry(const QString &isoCode)
    {
        const CCountry c(sGui->getWebDataServices()->getCountryForIsoCode(isoCode));
        this->setCountry(c);
    }

    BlackMisc::CCountry CDbCountrySelectorComponent::getCountry() const
    {
        if (!sGui) { return CCountry(); }
        const QString iso(ui->le_CountryIso->text().trimmed().toUpper());
        const QString name(ui->le_CountryName->text().trimmed());
        if (CCountry::isValidIsoCode(iso))
        {
            return sGui->getWebDataServices()->getCountryForIsoCode(iso);
        }
        else
        {
            if (name.isEmpty()) { return CCountry(); }
            return sGui->getWebDataServices()->getCountryForName(name);
        }
    }

    void CDbCountrySelectorComponent::setReadOnly(bool readOnly)
    {
        ui->le_CountryIso->setReadOnly(readOnly);
        ui->le_CountryName->setReadOnly(readOnly);
        ui->le_CountryName->setEnabled((!readOnly));
        ui->lbl_CountryIcon->setVisible(!readOnly);
        this->setEnabled(!readOnly);
    }

    bool CDbCountrySelectorComponent::isSet() const
    {
        return this->getCountry().isValid();
    }

    void CDbCountrySelectorComponent::clear()
    {
        ui->le_CountryIso->clear();
        ui->le_CountryName->clear();
        ui->lbl_CountryIcon->setPixmap(QPixmap());
    }

    void CDbCountrySelectorComponent::dragEnterEvent(QDragEnterEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        setBackgroundRole(QPalette::Highlight);
        event->acceptProposedAction();
    }

    void CDbCountrySelectorComponent::dragMoveEvent(QDragMoveEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        event->acceptProposedAction();
    }

    void CDbCountrySelectorComponent::dragLeaveEvent(QDragLeaveEvent *event)
    {
        if (!event) { return; }
        event->accept();
    }

    void CDbCountrySelectorComponent::dropEvent(QDropEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        const CVariant valueVariant(toCVariant(event->mimeData()));
        if (valueVariant.isValid())
        {
            if (valueVariant.canConvert<CCountry>())
            {
                const CCountry country(valueVariant.value<CCountry>());
                if (!country.hasIsoCode()) { return; }
                this->setCountry(country);
            }
            else if (valueVariant.canConvert<CCountryList>())
            {
                const CCountryList countries(valueVariant.value<CCountryList>());
                if (countries.isEmpty()) { return; }
                this->setCountry(countries.front());
            }
        }
    }

    void CDbCountrySelectorComponent::onCountriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        if (entity.testFlag(CEntityFlags::DistributorEntity) && CEntityFlags::isFinishedReadState(readState))
        {
            if (count > 0)
            {
                QCompleter *c = new QCompleter(sGui->getWebDataServices()->getCountries().toNameList(), this);
                c->setCaseSensitivity(Qt::CaseInsensitive);
                c->setCompletionMode(QCompleter::PopupCompletion);
                c->setMaxVisibleItems(10);
                connect(c, qOverload<const QString &>(&QCompleter::activated), this, &CDbCountrySelectorComponent::onCompleterActivated);

                ui->le_CountryName->setCompleter(c);
                m_completerCountryNames.reset(c); // deletes any old completer
            }
            else
            {
                m_completerCountryNames.reset(nullptr);
            }
        }
    }

    void CDbCountrySelectorComponent::onDataChanged()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        QObject *sender = this->sender();
        if (sender == ui->le_CountryIso)
        {
            QString iso(ui->le_CountryIso->text().trimmed().toUpper());
            if (CCountry::isValidIsoCode(iso))
            {
                this->setCountry(sGui->getWebDataServices()->getCountryForIsoCode(iso));
            }
        }
        else if (sender == ui->le_CountryName)
        {
            QString name(ui->le_CountryName->text().trimmed());
            if (!name.isEmpty())
            {
                this->setCountry(sGui->getWebDataServices()->getCountryForName(name));
            }
        }
    }

    void CDbCountrySelectorComponent::onCompleterActivated(const QString &countryName)
    {
        ui->le_CountryName->setText(countryName);
        this->setCountry(sGui->getWebDataServices()->getCountryForName(countryName));
    }

} // ns
