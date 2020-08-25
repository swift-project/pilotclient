/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/dbliveryselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"
#include "ui_dbliveryselectorcomponent.h"

#include <QCompleter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMetaType>
#include <QPalette>
#include <QStringList>
#include <Qt>
#include <QtGlobal>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbLiverySelectorComponent::CDbLiverySelectorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbLiverySelectorComponent)
        {
            ui->setupUi(this);
            this->setAcceptDrops(true);
            this->setAcceptedMetaTypeIds({qMetaTypeId<CLivery>(), qMetaTypeId<CLiveryList>()});

            ui->le_Livery->setValidator(new CUpperCaseValidator(this));

            connect(ui->le_Livery, &QLineEdit::returnPressed,   this, &CDbLiverySelectorComponent::onDataChanged);
            connect(ui->le_Livery, &QLineEdit::editingFinished, this, &CDbLiverySelectorComponent::onDataChanged);

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbLiverySelectorComponent::onLiveriesRead, Qt::QueuedConnection);
            this->onLiveriesRead(CEntityFlags::LiveryEntity, CEntityFlags::ReadFinished, sGui->getWebDataServices()->getLiveriesCount(), {});
        }

        CDbLiverySelectorComponent::~CDbLiverySelectorComponent()
        {
            // void
        }

        void CDbLiverySelectorComponent::setLivery(const CLivery &livery)
        {
            if (!livery.hasCombinedCode())
            {
                ui->le_Livery->clear();
                return;
            }

            if (livery != m_currentLivery)
            {
                ui->le_Livery->setText(livery.getCombinedCodePlusId());
                m_currentLivery = livery;
                emit changedLivery(livery);
            }
        }

        void CDbLiverySelectorComponent::setLivery(const QString &code)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
            const int dbKey = CDatastoreUtility::extractIntegerKey(code);
            CLivery livery;

            if (dbKey >= 0)
            {
                livery = sGui->getWebDataServices()->getLiveryForDbKey(dbKey);
            }

            if (!livery.hasValidDbKey())
            {
                const QString liveryCode = this->stripExtraInfo(code.toUpper().trimmed());
                if (m_currentLivery.matchesCombinedCode(liveryCode)) { return; }
                livery = sGui->getWebDataServices()->getLiveries().findByCombinedCode(liveryCode);
            }

            if (livery.hasCompleteData())
            {
                this->setLivery(livery);
            }
            else
            {
                ui->lbl_Description->setText("");
                ui->le_Livery->setText(code);
            }
        }

        CLivery CDbLiverySelectorComponent::getLivery() const
        {
            if (!sGui || sGui->isShuttingDown()) { return CLivery(); }

            const QString raw = ui->le_Livery->text();
            const int dbKey = CDatastoreUtility::extractIntegerKey(raw);

            CLivery livery;
            if (dbKey >= 0)
            {
                livery = sGui->getWebDataServices()->getLiveryForDbKey(dbKey);
            }
            else
            {
                const QString liveryCode(this->stripExtraInfo(ui->le_Livery->text()));
                livery = sGui->getWebDataServices()->getLiveries().findByCombinedCode(liveryCode);
            }

            if (livery.hasCompleteData() && livery.hasValidDbKey())
            {
                // full data fetched
                return livery;
            }

            return m_currentLivery;
        }

        QString CDbLiverySelectorComponent::getRawCombinedCode() const
        {
            const QString cc(ui->le_Livery->text().trimmed().toUpper());
            return stripDesignatorFromCompleterString(cc);
        }

        void CDbLiverySelectorComponent::setReadOnly(bool readOnly)
        {
            ui->le_Livery->setReadOnly(readOnly);
        }

        void CDbLiverySelectorComponent::withLiveryDescription(bool description)
        {
            ui->lbl_Description->setVisible(description);
        }

        bool CDbLiverySelectorComponent::isSet() const
        {
            return this->getLivery().hasCompleteData();
        }

        void CDbLiverySelectorComponent::clear()
        {
            ui->le_Livery->clear();
        }

        void CDbLiverySelectorComponent::dragEnterEvent(QDragEnterEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            this->setBackgroundRole(QPalette::Highlight);
            event->acceptProposedAction();
        }

        void CDbLiverySelectorComponent::dragMoveEvent(QDragMoveEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            event->acceptProposedAction();
        }

        void CDbLiverySelectorComponent::dragLeaveEvent(QDragLeaveEvent *event)
        {
            if (!event) { return; }
            event->accept();
        }

        void CDbLiverySelectorComponent::dropEvent(QDropEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            const CVariant valueVariant(toCVariant(event->mimeData()));
            if (valueVariant.isValid())
            {
                if (valueVariant.canConvert<CLivery>())
                {
                    const CLivery livery(valueVariant.value<CLivery>());
                    if (!livery.hasValidDbKey()) { return; }
                    this->setLivery(livery);
                }
                else if (valueVariant.canConvert<CLiveryList>())
                {
                    const CLiveryList liveries(valueVariant.value<CLiveryList>());
                    if (liveries.isEmpty()) { return; }
                    this->setLivery(liveries.front());
                }
            }
        }

        void CDbLiverySelectorComponent::onLiveriesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count, const QUrl &url)
        {
            Q_UNUSED(url)

            if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
            if (entity.testFlag(CEntityFlags::LiveryEntity) && CEntityFlags::isFinishedReadState(readState))
            {
                if (count > 0)
                {
                    const QStringList codes(sGui->getWebDataServices()->getLiveries().getCombinedCodesPlusInfoAndId(true));
                    QCompleter *c = new QCompleter(codes, this);
                    c->setCaseSensitivity(Qt::CaseInsensitive);
                    c->setCompletionMode(QCompleter::PopupCompletion);
                    c->setMaxVisibleItems(10);
                    connect(c, qOverload<const QString &>(&QCompleter::activated), this, &CDbLiverySelectorComponent::onCompleterActivated);

                    ui->le_Livery->setCompleter(c);
                    m_completerLiveries.reset(c); // deletes any old completer
                }
                else
                {
                    m_completerLiveries.reset(nullptr);
                }
            }
        }

        void CDbLiverySelectorComponent::onDataChanged()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
            const QString raw = ui->le_Livery->text();
            this->setLivery(raw);
        }

        void CDbLiverySelectorComponent::onCompleterActivated(const QString &liveryCode)
        {
            this->setLivery(liveryCode);
        }

        QString CDbLiverySelectorComponent::stripExtraInfo(const QString &liveryCode) const
        {
            if (liveryCode.isEmpty()) { return {}; }
            const QString l(liveryCode.trimmed().toUpper());
            int is = l.indexOf(' ');
            int ib = l.indexOf('(');
            int i = qMin(is, ib);
            if (i < 0) { return l; }
            return l.left(i);
        }
    } // ns
} // ns
