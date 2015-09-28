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
#include "blackmisc/datastoreutility.h"
#include <QMimeData>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbAirlineIcaoSelectorComponent::CDbAirlineIcaoSelectorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAirlineIcaoSelectorComponent)
        {
            ui->setupUi(this);
            this->setAcceptDrops(true);
            this->setAcceptedMetaTypeIds({qMetaTypeId<CAirlineIcaoCode>(), qMetaTypeId<CAirlineIcaoCodeList>()});

            connect(ui->le_Airline, &QLineEdit::returnPressed, this, &CDbAirlineIcaoSelectorComponent::ps_dataChanged);
        }

        CDbAirlineIcaoSelectorComponent::~CDbAirlineIcaoSelectorComponent()
        {
            gracefulShutdown();
        }

        void CDbAirlineIcaoSelectorComponent::setProvider(Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            if (!webDataReaderProvider) { return; }
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectSwiftDatabaseSignals(
                this,
                std::bind(&CDbAirlineIcaoSelectorComponent::ps_codesRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = this->getAirlineIcaoCodesCount();
            if (c > 0)
            {
                this->ps_codesRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbAirlineIcaoSelectorComponent::setAirlineIcao(const CAirlineIcaoCode &icao)
        {
            QString icaoStr(icao.getVDesignator());
            this->ui->le_Airline->setText(icaoStr);
            ui->lbl_Description->setText(icao.getName());
            if (icao != m_currentIcao)
            {
                m_currentIcao = icao;
                emit changedAirlineIcao(icao);
            }
        }

        void CDbAirlineIcaoSelectorComponent::setAirlineIcao(int key)
        {
            CAirlineIcaoCode icao(getAirlineIcaoCodeForDbKey(key));
            ui->lbl_Description->setText("");
            if (icao.hasCompleteData())
            {
                this->setAirlineIcao(icao);
            }
        }

        CAirlineIcaoCode CDbAirlineIcaoSelectorComponent::getAirlineIcao() const
        {
            int key = CDatastoreUtility::extractIntegerKey(this->ui->le_Airline->text());
            if (key < 0) { return CAirlineIcaoCode(); }
            CAirlineIcaoCode icao(getAirlineIcaoCodeForDbKey(key));
            return icao;
        }

        void CDbAirlineIcaoSelectorComponent::setReadOnly(bool readOnly)
        {
            this->ui->le_Airline->setReadOnly(readOnly);
        }

        void CDbAirlineIcaoSelectorComponent::withIcaoDescription(bool description)
        {
            this->ui->lbl_Description->setVisible(description);
        }

        bool CDbAirlineIcaoSelectorComponent::isSet() const
        {
            return this->getAirlineIcao().hasCompleteData();
        }

        void CDbAirlineIcaoSelectorComponent::clear()
        {
            this->ui->le_Airline->clear();
        }

        void CDbAirlineIcaoSelectorComponent::dragEnterEvent(QDragEnterEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            setBackgroundRole(QPalette::Highlight);
            event->acceptProposedAction();
        }

        void CDbAirlineIcaoSelectorComponent::dragMoveEvent(QDragMoveEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            event->acceptProposedAction();
        }

        void CDbAirlineIcaoSelectorComponent::dragLeaveEvent(QDragLeaveEvent *event)
        {
            if (!event) { return; }
            event->accept();
        }

        void CDbAirlineIcaoSelectorComponent::dropEvent(QDropEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            CVariant valueVariant(toCVariant(event->mimeData()));
            if (valueVariant.isValid())
            {
                if (valueVariant.canConvert<CAirlineIcaoCode>())
                {
                    CAirlineIcaoCode icao(valueVariant.value<CAirlineIcaoCode>());
                    if (!icao.hasValidDbKey()) { return; }
                    this->setAirlineIcao(icao);
                }
                else if (valueVariant.canConvert<CAirlineIcaoCodeList>())
                {
                    CAirlineIcaoCodeList icaos(valueVariant.value<CAirlineIcaoCodeList>());
                    if (icaos.isEmpty()) { return; }
                    this->setAirlineIcao(icaos.front());
                }
            }
        }

        void CDbAirlineIcaoSelectorComponent::ps_codesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            if (!hasProvider()) { return; }
            if (entity.testFlag(CEntityFlags::AirlineIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                if (count > 0)
                {
                    QCompleter *c = new QCompleter(this->getAirlineIcaoCodes().toCompleterStrings(), this);
                    c->setCaseSensitivity(Qt::CaseInsensitive);
                    c->setCompletionMode(QCompleter::PopupCompletion);
                    c->setMaxVisibleItems(10);
                    this->connect(c, static_cast<void (QCompleter::*)(const QString &)>(&QCompleter::activated), this, &CDbAirlineIcaoSelectorComponent::ps_completerActivated);

                    this->ui->le_Airline->setCompleter(c);
                    m_completerIcaoDescription.reset(c); // deletes any old completer
                    this->setReadOnly(false);
                }
                else
                {
                    this->m_completerIcaoDescription.reset(nullptr);
                    this->setReadOnly(true);
                }
            }
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
                // second choice, first object found by designator
                CAirlineIcaoCode icao(getAirlineIcaoCodeForDesignator(s));
                this->setAirlineIcao(icao);
            }
        }

        void CDbAirlineIcaoSelectorComponent::ps_completerActivated(const QString &icaoString)
        {
            int dbKey = CDatastoreUtility::extractIntegerKey(icaoString);
            if (dbKey < 0) { return; }
            this->setAirlineIcao(dbKey);
        }

    }// class
} // ns
