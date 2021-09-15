/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_dbaircrafticaoselectorcomponent.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/dbaircrafticaoselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QCompleter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMetaType>
#include <QPalette>
#include <Qt>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CDbAircraftIcaoSelectorComponent::CDbAircraftIcaoSelectorComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CDbAircraftIcaoSelectorComponent)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->le_Aircraft);
        this->setAcceptDrops(true);
        this->setAcceptedMetaTypeIds({qMetaTypeId<CAircraftIcaoCode>(), qMetaTypeId<CAircraftIcaoCodeList>()});
        ui->le_Aircraft->setValidator(new CUpperCaseValidator(this));

        connect(ui->le_Aircraft, &QLineEdit::editingFinished, this, &CDbAircraftIcaoSelectorComponent::onDataChanged);
        if (sApp && sApp->hasWebDataServices())
        {
            connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftIcaoSelectorComponent::onCodesRead, Qt::QueuedConnection);
            this->onCodesRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, sApp->getWebDataServices()->getAircraftIcaoCodesCount());
        }
    }

    CDbAircraftIcaoSelectorComponent::~CDbAircraftIcaoSelectorComponent()
    { }

    bool CDbAircraftIcaoSelectorComponent::setAircraftIcao(const CAircraftIcaoCode &icao)
    {
        CAircraftIcaoCode setIcao(icao);
        if (!icao.isLoadedFromDb())
        {
            // resolve against DB
            setIcao = sGui->getWebDataServices()->smartAircraftIcaoSelector(icao);
        }

        const QString icaoStr(m_display == DisplayIcaoAndId ? setIcao.getDesignatorDbKey() : setIcao.getCombinedIcaoStringWithKey());
        ui->le_Aircraft->setText(icaoStr);
        ui->lbl_Description->setText(setIcao.getManufacturer());
        if (setIcao == m_currentIcao) { return false; }

        m_currentIcao = setIcao;
        emit changedAircraftIcao(setIcao);
        return true;
    }

    bool CDbAircraftIcaoSelectorComponent::setAircraftIcao(int key)
    {
        const CAircraftIcaoCode icao(sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(key));
        ui->lbl_Description->setText("");
        return icao.hasCompleteData() ? this->setAircraftIcao(icao) : false;
    }

    CAircraftIcaoCode CDbAircraftIcaoSelectorComponent::getAircraftIcao() const
    {
        const QString text(ui->le_Aircraft->text().trimmed().toUpper());
        const int key = CDatastoreUtility::extractIntegerKey(text);
        if (key < 0)
        {
            const QString icaoOnly = CDatastoreUtility::stripKeyInParentheses(text);
            if (m_currentIcao.getDesignator() == icaoOnly) { return m_currentIcao; }
            return CAircraftIcaoCode(icaoOnly);
        }
        CAircraftIcaoCode icao(sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(key));
        if (icao.isNull())
        {
            // did not find by key
            const QString icaoOnly = CDatastoreUtility::stripKeyInParentheses(text);
            if (m_currentIcao.getDesignator() == icaoOnly) { return m_currentIcao; }
            return CAircraftIcaoCode(icaoOnly);
        }
        return icao;
    }

    void CDbAircraftIcaoSelectorComponent::setReadOnly(bool readOnly)
    {
        ui->le_Aircraft->setReadOnly(readOnly);
    }

    QString CDbAircraftIcaoSelectorComponent::getRawDesignator() const
    {
        return stripDesignatorFromCompleterString(ui->le_Aircraft->text());
    }

    void CDbAircraftIcaoSelectorComponent::displayWithIcaoDescription(bool description)
    {
        ui->lbl_Description->setVisible(description);
    }

    bool CDbAircraftIcaoSelectorComponent::isSet() const
    {
        const CAircraftIcaoCode icao(this->getAircraftIcao());
        return icao.isLoadedFromDb() || icao.hasCompleteData();
    }

    void CDbAircraftIcaoSelectorComponent::clear()
    {
        ui->le_Aircraft->clear();
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
        const CVariant valueVariant(toCVariant(event->mimeData()));
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

    const QStringList &CDbAircraftIcaoSelectorComponent::completerStrings()
    {
        static const QStringList empty;
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return empty; }
        const int c = sGui->getWebDataServices()->getAircraftIcaoCodesCount();
        if (c != m_completerStrings.size())
        {
            CAircraftIcaoCodeList icaos(sGui->getWebDataServices()->getAircraftIcaoCodes());
            icaos.removeInvalidCombinedCodes();
            m_completerStrings = icaos.toCompleterStrings(true, true, true, true);
        }
        return m_completerStrings;
    }

    void CDbAircraftIcaoSelectorComponent::onCodesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) && CEntityFlags::isFinishedReadState(readState))
        {
            if (count > 0)
            {
                QCompleter *c = new QCompleter(this->completerStrings(), this);
                c->setCaseSensitivity(Qt::CaseInsensitive);
                c->setCompletionMode(QCompleter::PopupCompletion);
                c->setMaxVisibleItems(10);
                const int w5chars = c->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
                c->popup()->setMinimumWidth(w5chars * 10);

                connect(c, qOverload<const QString &>(&QCompleter::activated), this, &CDbAircraftIcaoSelectorComponent::onCompleterActivated);

                ui->le_Aircraft->setCompleter(c);
                m_completerIcaoDescription.reset(c); // deletes any old completer
                this->setReadOnly(false);
            }
            else
            {
                m_completerIcaoDescription.reset(nullptr);
            }
        }
    }

    void CDbAircraftIcaoSelectorComponent::onDataChanged()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const int key = CDatastoreUtility::extractIntegerKey(ui->le_Aircraft->text());
        CAircraftIcaoCode icao;
        if (key >= 0)
        {
            // with valid key
            icao = sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(key);
        }
        else
        {
            const QString designator(this->getRawDesignator());
            icao = sGui->getWebDataServices()->smartAircraftIcaoSelector(CAircraftIcaoCode(designator));
        }
        this->setAircraftIcao(icao);
    }

    void CDbAircraftIcaoSelectorComponent::onCompleterActivated(const QString &icaoString)
    {
        const int dbKey = CDatastoreUtility::extractIntegerKey(icaoString);
        if (dbKey < 0) { return; }
        this->setAircraftIcao(dbKey);
    }
} // ns
