// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbdistributorselectorcomponent.h"

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

#include "ui_dbdistributorselectorcomponent.h"

#include "core/application.h"
#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "misc/mixin/mixincompare.h"
#include "misc/simulation/distributorlist.h"
#include "misc/variant.h"

using namespace swift::gui;
using namespace swift::core;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::network;

namespace swift::gui::components
{
    CDbDistributorSelectorComponent::CDbDistributorSelectorComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CDbDistributorSelectorComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        ui->setupUi(this);
        this->setFocusProxy(ui->le_Distributor);
        this->setAcceptDrops(true);
        this->setAcceptedMetaTypeIds({ qMetaTypeId<CDistributor>(), qMetaTypeId<CDistributorList>() });
        ui->le_Distributor->setValidator(new CUpperCaseValidator(this));

        bool c = connect(ui->le_Distributor, &QLineEdit::editingFinished, this,
                         &CDbDistributorSelectorComponent::onDataChanged, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        // c = connect(ui->le_Distributor, &QLineEdit::returnPressed, this,
        // &CDbDistributorSelectorComponent::returnPressed, Qt::QueuedConnection); Q_ASSERT_X(c, Q_FUNC_INFO, "Missing
        // connect");
        c = connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this,
                    &CDbDistributorSelectorComponent::onDistributorsRead, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Missing connect");
        Q_UNUSED(c);

        this->onDistributorsRead(CEntityFlags::DistributorEntity, CEntityFlags::ReadFinished,
                                 sApp->getWebDataServices()->getDistributorsCount());
    }

    CDbDistributorSelectorComponent::~CDbDistributorSelectorComponent() = default;

    void CDbDistributorSelectorComponent::setDistributor(const CDistributor &distributor)
    {
        const QString key(distributor.getDbKey());
        if (key.isEmpty()) { return; }
        if (distributor != m_currentDistributor)
        {
            ui->le_Distributor->setText(key);
            m_currentDistributor = distributor;
            emit changedDistributor(distributor);
        }
    }

    void CDbDistributorSelectorComponent::setDistributor(const QString &distributorKeyOrAlias)
    {
        QString keyOrAlias(distributorKeyOrAlias.toUpper().trimmed());
        if (m_currentDistributor.matchesKeyOrAlias(keyOrAlias)) { return; }
        CDistributor d(sGui->getWebDataServices()->getDistributors().findByKeyOrAlias(keyOrAlias));
        if (d.hasCompleteData()) { this->setDistributor(d); }
        else
        {
            ui->lbl_Description->setText("");
            ui->le_Distributor->setText(keyOrAlias);
        }
    }

    CDistributor CDbDistributorSelectorComponent::getDistributor() const
    {
        if (!sGui) { return CDistributor(); }
        const QString distributorKeyOrAlias(ui->le_Distributor->text().trimmed().toUpper());
        if (distributorKeyOrAlias.isEmpty()) { return CDistributor(); }
        if (m_currentDistributor.matchesKeyOrAlias(distributorKeyOrAlias)) { return m_currentDistributor; }

        const CDistributor d(sGui->getWebDataServices()->getDistributors().findByKey(distributorKeyOrAlias));
        if (d.hasValidDbKey())
        {
            // for some reason we have a new distributor here
            // normally that should not happen
        }
        return d;
    }

    void CDbDistributorSelectorComponent::setReadOnly(bool readOnly) { ui->le_Distributor->setReadOnly(readOnly); }

    void CDbDistributorSelectorComponent::withDistributorDescription(bool description)
    {
        ui->lbl_Description->setVisible(description);
    }

    bool CDbDistributorSelectorComponent::isSet() const { return this->getDistributor().hasCompleteData(); }

    void CDbDistributorSelectorComponent::clear()
    {
        ui->le_Distributor->clear();
        m_currentDistributor = CDistributor();
    }

    void CDbDistributorSelectorComponent::dragEnterEvent(QDragEnterEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        setBackgroundRole(QPalette::Highlight);
        event->acceptProposedAction();
    }

    void CDbDistributorSelectorComponent::dragMoveEvent(QDragMoveEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        event->acceptProposedAction();
    }

    void CDbDistributorSelectorComponent::dragLeaveEvent(QDragLeaveEvent *event)
    {
        if (!event) { return; }
        event->accept();
    }

    void CDbDistributorSelectorComponent::dropEvent(QDropEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        CVariant valueVariant(toCVariant(event->mimeData()));
        if (valueVariant.isValid())
        {
            if (valueVariant.canConvert<CDistributor>())
            {
                auto distributor(valueVariant.value<CDistributor>());
                if (!distributor.hasValidDbKey()) { return; }
                this->setDistributor(distributor);
            }
            else if (valueVariant.canConvert<CDistributorList>())
            {
                auto distributors(valueVariant.value<CDistributorList>());
                if (distributors.isEmpty()) { return; }
                this->setDistributor(distributors.front());
            }
        }
    }

    void CDbDistributorSelectorComponent::onDistributorsRead(CEntityFlags::Entity entity,
                                                             CEntityFlags::ReadState readState, int count)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        if (entity.testFlag(CEntityFlags::DistributorEntity) && CEntityFlags::isFinishedReadState(readState))
        {
            if (count > 0)
            {
                const QStringList keysAndAliases(
                    sGui->getWebDataServices()->getDistributors().getDbKeysAndAliases(true));
                auto *c = new QCompleter(keysAndAliases, this);
                c->setCaseSensitivity(Qt::CaseInsensitive);
                c->setCompletionMode(QCompleter::PopupCompletion);
                c->setMaxVisibleItems(10);
                connect(c, qOverload<const QString &>(&QCompleter::activated), this,
                        &CDbDistributorSelectorComponent::onCompleterActivated);

                ui->le_Distributor->setCompleter(c);
                m_completerDistributors.reset(c); // deletes any old completer
                this->setReadOnly(false);
            }
            else
            {
                m_completerDistributors.reset(nullptr);
                this->setReadOnly(true);
            }
        }
    }

    void CDbDistributorSelectorComponent::onDataChanged()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        const QString keyOrAlias(ui->le_Distributor->text().trimmed().toUpper());
        if (keyOrAlias.isEmpty()) { return; }
        const CDistributor distributor(sGui->getWebDataServices()->getDistributors().findByKeyOrAlias(keyOrAlias));
        this->setDistributor(distributor);
    }

    void CDbDistributorSelectorComponent::onCompleterActivated(const QString &distributorKeyOrAlias)
    {
        this->setDistributor(distributorKeyOrAlias);
    }

} // namespace swift::gui::components
