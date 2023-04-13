/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/dbairlineicaoselectorbase.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/variant.h"

#include <QCompleter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMetaType>
#include <QPalette>
#include <QtGlobal>
#include <QPointer>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CDbAirlineIcaoSelectorBase::CDbAirlineIcaoSelectorBase(QWidget *parent) : QFrame(parent)
    {
        this->setAcceptDrops(true);
        this->setAcceptedMetaTypeIds({ qMetaTypeId<CAirlineIcaoCode>(), qMetaTypeId<CAirlineIcaoCodeList>() });

        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAirlineIcaoSelectorBase::onCodesRead, Qt::QueuedConnection);

        // when we already have data, init completers. This can not be done directly in the
        // constructor due to virtual functions
        const int c = sGui->getWebDataServices()->getAirlineIcaoCodesCount();
        if (c > 0)
        {
            QPointer<CDbAirlineIcaoSelectorBase> myself(this);
            QTimer::singleShot(500, [=]() {
                if (!sGui || sGui->isShuttingDown()) { return; }
                if (!myself) { return; }
                this->onCodesRead(CEntityFlags::AirlineIcaoEntity, CEntityFlags::ReadFinished, c, {});
            });
        }
    }

    CDbAirlineIcaoSelectorBase::~CDbAirlineIcaoSelectorBase()
    {}

    bool CDbAirlineIcaoSelectorBase::setAirlineIcao(const CAirlineIcaoCode &icao)
    {
        if (icao == m_currentIcao) { return false; }
        if (icao.isLoadedFromDb())
        {
            m_currentIcao = icao;
        }
        else
        {
            m_currentIcao = sGui->getWebDataServices()->smartAirlineIcaoSelector(icao);
        }
        emit changedAirlineIcao(m_currentIcao);
        return true;
    }

    bool CDbAirlineIcaoSelectorBase::setAirlineIcao(int key)
    {
        CAirlineIcaoCode icao(sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(key));
        if (!icao.hasCompleteData()) { return false; }
        this->setAirlineIcao(icao);
        return true;
    }

    bool CDbAirlineIcaoSelectorBase::isSet() const
    {
        const CAirlineIcaoCode icao(this->getAirlineIcao());
        return icao.isLoadedFromDb() || icao.hasCompleteData();
    }

    void CDbAirlineIcaoSelectorBase::dragEnterEvent(QDragEnterEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        this->setBackgroundRole(QPalette::Highlight);
        event->acceptProposedAction();
    }

    void CDbAirlineIcaoSelectorBase::dragMoveEvent(QDragMoveEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        event->acceptProposedAction();
    }

    void CDbAirlineIcaoSelectorBase::dragLeaveEvent(QDragLeaveEvent *event)
    {
        if (!event) { return; }
        event->accept();
    }

    void CDbAirlineIcaoSelectorBase::dropEvent(QDropEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        const CVariant valueVariant(toCVariant(event->mimeData()));
        if (valueVariant.isValid())
        {
            if (valueVariant.canConvert<CAirlineIcaoCode>())
            {
                const CAirlineIcaoCode icao(valueVariant.value<CAirlineIcaoCode>());
                if (!icao.hasValidDbKey()) { return; }
                this->setAirlineIcao(icao);
            }
            else if (valueVariant.canConvert<CAirlineIcaoCodeList>())
            {
                const CAirlineIcaoCodeList icaos(valueVariant.value<CAirlineIcaoCodeList>());
                if (icaos.isEmpty()) { return; }
                this->setAirlineIcao(icaos.front());
            }
        }
    }

    void CDbAirlineIcaoSelectorBase::onCodesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count, const QUrl &url)
    {
        Q_UNUSED(url)

        if (!sGui) { return; }
        if (entity.testFlag(CEntityFlags::AirlineIcaoEntity) && CEntityFlags::isFinishedReadState(readState))
        {
            if (count > 0)
            {
                QCompleter *c = this->createCompleter();
                Q_ASSERT_X(c, Q_FUNC_INFO, "missing converter");
                connect(c, qOverload<const QString &>(&QCompleter::activated), this, &CDbAirlineIcaoSelectorBase::onCompleterActivated);
                m_completer.reset(c); // deletes any old completer
            }
            else
            {
                m_completer.reset(nullptr);
            }
        }
    }

    void CDbAirlineIcaoSelectorBase::onCompleterActivated(const QString &icaoString)
    {
        const int dbKey = CDatastoreUtility::extractIntegerKey(icaoString);
        if (dbKey < 0) { return; }
        this->setAirlineIcao(dbKey);
    }
} // ns
