/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbaircrafticaoselectorcomponent.h"
#include "ui_dbaircrafticaoselectorcomponent.h"
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
        CDbAircraftIcaoSelectorComponent::CDbAircraftIcaoSelectorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAircraftIcaoSelectorComponent)
        {
            ui->setupUi(this);
            this->setAcceptDrops(true);
            this->setAcceptedMetaTypeIds({qMetaTypeId<CAircraftIcaoCode>(), qMetaTypeId<CAircraftIcaoCodeList>()});

            connect(ui->le_Aircraft, &QLineEdit::returnPressed, this, &CDbAircraftIcaoSelectorComponent::ps_dataChanged);
        }

        CDbAircraftIcaoSelectorComponent::~CDbAircraftIcaoSelectorComponent()
        { }

        void CDbAircraftIcaoSelectorComponent::setProvider(Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            if (!webDataReaderProvider) { return; }
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            connectDataReadSignal(
                this,
                std::bind(&CDbAircraftIcaoSelectorComponent::ps_codesRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            );
            int c = this->getAircraftIcaoCodesCount();
            if (c > 0)
            {
                this->ps_codesRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, c);
            }
        }

        void CDbAircraftIcaoSelectorComponent::setAircraftIcao(const CAircraftIcaoCode &icao)
        {
            QString icaoStr(icao.getDesignator());
            this->ui->le_Aircraft->setText(icaoStr);
            ui->lbl_Description->setText(icao.getManufacturer());
            if (icao != m_currentIcao)
            {
                m_currentIcao = icao;
                emit changedAircraftIcao(icao);
            }
        }

        void CDbAircraftIcaoSelectorComponent::setAircraftIcao(int key)
        {
            CAircraftIcaoCode icao(getAircraftIcaoCodeForDbKey(key));
            ui->lbl_Description->setText("");
            if (icao.hasCompleteData())
            {
                this->setAircraftIcao(icao);
            }
        }

        CAircraftIcaoCode CDbAircraftIcaoSelectorComponent::getAircraftIcao() const
        {
            int key = CDatastoreUtility::extractIntegerKey(this->ui->le_Aircraft->text());
            if (key < 0) { return CAircraftIcaoCode(); }
            CAircraftIcaoCode icao(getAircraftIcaoCodeForDbKey(key));
            return icao;
        }

        void CDbAircraftIcaoSelectorComponent::setReadOnly(bool readOnly)
        {
            this->ui->le_Aircraft->setReadOnly(readOnly);
        }

        void CDbAircraftIcaoSelectorComponent::withIcaoDescription(bool description)
        {
            this->ui->lbl_Description->setVisible(description);
        }

        bool CDbAircraftIcaoSelectorComponent::isSet() const
        {
            return this->getAircraftIcao().hasCompleteData();
        }

        void CDbAircraftIcaoSelectorComponent::clear()
        {
            this->ui->le_Aircraft->clear();
        }

        void CDbAircraftIcaoSelectorComponent::dragEnterEvent(QDragEnterEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            setBackgroundRole(QPalette::Highlight);
            event->acceptProposedAction();
        }

        void CDbAircraftIcaoSelectorComponent::dragMoveEvent(QDragMoveEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            event->acceptProposedAction();
        }

        void CDbAircraftIcaoSelectorComponent::dragLeaveEvent(QDragLeaveEvent *event)
        {
            if (!event) { return; }
            event->accept();
        }

        void CDbAircraftIcaoSelectorComponent::dropEvent(QDropEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            CVariant valueVariant(toCVariant(event->mimeData()));
            if (valueVariant.isValid())
            {
                if (valueVariant.canConvert<CAircraftIcaoCode>())
                {
                    CAircraftIcaoCode icao(valueVariant.value<CAircraftIcaoCode>());
                    if (!icao.hasValidDbKey()) { return; }
                    this->setAircraftIcao(icao);
                }
                else if (valueVariant.canConvert<CAircraftIcaoCodeList>())
                {
                    CAircraftIcaoCodeList icaos(valueVariant.value<CAircraftIcaoCodeList>());
                    if (icaos.isEmpty()) { return; }
                    this->setAircraftIcao(icaos.front());
                }
            }
        }

        void CDbAircraftIcaoSelectorComponent::ps_codesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            if (!hasProvider()) { return; }
            if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                if (count > 0)
                {
                    QCompleter *c = new QCompleter(this->getAircraftIcaoCodes().toCompleterStrings(), this);
                    c->setCaseSensitivity(Qt::CaseInsensitive);
                    c->setCompletionMode(QCompleter::PopupCompletion);
                    c->setMaxVisibleItems(10);
                    this->connect(c, static_cast<void (QCompleter::*)(const QString &)>(&QCompleter::activated), this, &CDbAircraftIcaoSelectorComponent::ps_completerActivated);

                    this->ui->le_Aircraft->setCompleter(c);
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

        void CDbAircraftIcaoSelectorComponent::ps_dataChanged()
        {
            if (!hasProvider()) { return; }
            int key = CDatastoreUtility::extractIntegerKey(this->ui->le_Aircraft->text());
            CAircraftIcaoCode icao(getAircraftIcaoCodeForDbKey(key));
            this->setAircraftIcao(icao);
        }

        void CDbAircraftIcaoSelectorComponent::ps_completerActivated(const QString &icaoString)
        {
            int dbKey = CDatastoreUtility::extractIntegerKey(icaoString);
            if (dbKey < 0) { return; }
            this->setAircraftIcao(dbKey);
        }

    }// class
} // ns
