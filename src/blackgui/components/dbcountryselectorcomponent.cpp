/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbcountryselectorcomponent.h"
#include "ui_dbcountryselectorcomponent.h"
#include "blackgui/guiutility.h"
#include <QMimeData>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbCountrySelectorComponent::CDbCountrySelectorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbCountrySelectorComponent)
        {
            ui->setupUi(this);
            this->setAcceptDrops(true);
            this->setAcceptedMetaTypeIds({qMetaTypeId<CCountry>(), qMetaTypeId<CCountryList>()});

            connect(ui->le_CountryIso, &QLineEdit::returnPressed, this, &CDbCountrySelectorComponent::ps_dataChanged);
            connect(ui->le_CountryName, &QLineEdit::returnPressed, this, &CDbCountrySelectorComponent::ps_dataChanged);
            connect(ui->le_CountryIso, &QLineEdit::editingFinished, this, &CDbCountrySelectorComponent::ps_dataChanged);
            connect(ui->le_CountryName, &QLineEdit::returnPressed, this, &CDbCountrySelectorComponent::ps_dataChanged);
        }

        CDbCountrySelectorComponent::~CDbCountrySelectorComponent()
        {
            gracefulShutdown();
        }

        void CDbCountrySelectorComponent::setProvider(Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            if (!webDataReaderProvider) { return; }
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            webDataReaderProvider->connectSwiftDatabaseSignals(
                this,
                std::bind(&CDbCountrySelectorComponent::ps_CountriesRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = this->getCountriesCount();
            if (c > 0)
            {
                this->ps_CountriesRead(CEntityFlags::CountryEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbCountrySelectorComponent::setCountry(const BlackMisc::CCountry &country)
        {
            this->ui->le_CountryIso->setText(country.getIsoCode());
            this->ui->le_CountryName->setText(country.getName());
            this->ui->lbl_CountryIcon->setPixmap(country.toPixmap());
            if (country != m_currentCountry)
            {
                m_currentCountry = country;
                emit countryChanged(country);
            }
        }

        void CDbCountrySelectorComponent::setCountry(const QString &isoCode)
        {
            CCountry c(this->getCountryForIsoCode(isoCode));
            this->setCountry(c);
        }

        BlackMisc::CCountry CDbCountrySelectorComponent::getCountry() const
        {
            if (!hasProvider()) { return CCountry(); }
            QString iso(this->ui->le_CountryIso->text().trimmed().toUpper());
            QString name(this->ui->le_CountryName->text().trimmed());
            if (CCountry::isValidIsoCode(iso))
            {
                return this->getCountryForIsoCode(iso);
            }
            else
            {
                if (name.isEmpty()) { return CCountry(); }
                return this->getCountryForName(name);
            }
        }

        void CDbCountrySelectorComponent::setReadOnly(bool readOnly)
        {
            this->ui->le_CountryIso->setReadOnly(readOnly);
            this->ui->le_CountryName->setReadOnly(readOnly);
            this->ui->lbl_CountryIcon->setVisible(!readOnly);
        }

        bool CDbCountrySelectorComponent::isSet() const
        {
            return this->getCountry().isValid();
        }

        void CDbCountrySelectorComponent::clear()
        {
            this->ui->le_CountryIso->clear();
            this->ui->le_CountryName->clear();
            this->ui->lbl_CountryIcon->setPixmap(QPixmap());
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
            CVariant valueVariant(toCVariant(event->mimeData()));
            if (valueVariant.isValid())
            {
                if (valueVariant.canConvert<CCountry>())
                {
                    CCountry country(valueVariant.value<CCountry>());
                    if (!country.hasIsoCode()) { return; }
                    this->setCountry(country);
                }
                else if (valueVariant.canConvert<CCountryList>())
                {
                    CCountryList countries(valueVariant.value<CCountryList>());
                    if (countries.isEmpty()) { return; }
                    this->setCountry(countries.front());
                }
            }
        }

        void CDbCountrySelectorComponent::ps_CountriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            if (!hasProvider()) { return; }
            if (entity.testFlag(CEntityFlags::DistributorEntity) && readState == CEntityFlags::ReadFinished)
            {
                if (count > 0)
                {
                    QCompleter *c = new QCompleter(this->getCountries().toNameList(), this);
                    c->setCaseSensitivity(Qt::CaseInsensitive);
                    c->setCompletionMode(QCompleter::PopupCompletion);
                    c->setMaxVisibleItems(10);
                    this->connect(c, static_cast<void (QCompleter::*)(const QString &)>(&QCompleter::activated), this, &CDbCountrySelectorComponent::ps_completerActivated);

                    this->ui->le_CountryName->setCompleter(c);
                    m_completerCountryNames.reset(c); // deletes any old completer
                    this->setReadOnly(false);
                }
                else
                {
                    this->m_completerCountryNames.reset(nullptr);
                    this->setReadOnly(true);
                }
            }
        }

        void CDbCountrySelectorComponent::ps_dataChanged()
        {
            if (!hasProvider()) { return; }
            QObject *sender = this->sender();
            if (sender == this->ui->le_CountryIso)
            {
                QString iso(this->ui->le_CountryIso->text().trimmed().toUpper());
                if (CCountry::isValidIsoCode(iso))
                {
                    this->setCountry(getCountryForIsoCode(iso));
                }
            }
            else if (sender == this->ui->le_CountryName)
            {
                QString name(this->ui->le_CountryName->text().trimmed());
                if (!name.isEmpty())
                {
                    this->setCountry(getCountryForName(name));
                }
            }
        }

        void CDbCountrySelectorComponent::ps_completerActivated(const QString &countryName)
        {
            this->ui->le_CountryName->setText(countryName);
            this->setCountry(getCountryForName(countryName));
        }

    }// class
} // ns
