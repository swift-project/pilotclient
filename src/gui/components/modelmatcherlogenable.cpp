// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "modelmatcherlogenable.h"
#include "gui/guiapplication.h"
#include "ui_modelmatcherlogenable.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"

#include <QPointer>
#include <QTimer>

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc::network;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CModelMatcherLogEnable::CModelMatcherLogEnable(QWidget *parent) : QFrame(parent),
                                                                      ui(new Ui::CModelMatcherLogEnable)
    {
        ui->setupUi(this);
        connect(ui->cb_LogReverseLookup, &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged, Qt::QueuedConnection);
        connect(ui->cb_LogMatchingMessages, &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged, Qt::QueuedConnection);
        connect(ui->cb_LogDetailed, &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged, Qt::QueuedConnection);

        if (this->hasContexts())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::changedLogOrDebugSettings, this, &CModelMatcherLogEnable::valuesChanged, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CModelMatcherLogEnable::valuesChanged, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CModelMatcherLogEnable::connectionStatusChanged, Qt::QueuedConnection);
        }

        QPointer<CModelMatcherLogEnable> myself(this);
        QTimer::singleShot(5000, this, [=] {
            if (!myself) { return; }
            this->initGui();
        });
    }

    CModelMatcherLogEnable::~CModelMatcherLogEnable()
    {}

    bool CModelMatcherLogEnable::isReverseLookupLogEnabled() const
    {
        return ui->cb_LogReverseLookup->isChecked();
    }

    bool CModelMatcherLogEnable::isMatchingLogEnabled() const
    {
        return ui->cb_LogMatchingMessages->isChecked();
    }

    void CModelMatcherLogEnable::enabledCheckboxChanged(bool enabled)
    {
        if (!this->hasContexts()) { return; }
        const QObject *sender = QObject::sender();

        const bool detailed = (sender == ui->cb_LogDetailed) ? enabled : ui->cb_LogDetailed->isChecked();
        const bool reverse = (sender == ui->cb_LogReverseLookup) ? enabled : ui->cb_LogReverseLookup->isChecked();
        const bool matching = (sender == ui->cb_LogMatchingMessages) ? enabled : ui->cb_LogMatchingMessages->isChecked();
        const bool simplified = !detailed;

        if (sender == ui->cb_LogReverseLookup || sender == ui->cb_LogDetailed)
        {
            ReverseLookupLogging revLog = RevLogDisabled;
            if (reverse && simplified) { revLog = RevLogEnabledSimplified; }
            else if (reverse) { revLog = RevLogEnabled; }

            sGui->getIContextNetwork()->enableReverseLookupMessages(revLog);
        }

        if (sender == ui->cb_LogMatchingMessages || sender == ui->cb_LogDetailed)
        {
            MatchingLog matchingLog = MatchingLogNothing;
            if (matching && simplified) { matchingLog = MatchingLogSimplified; }
            else if (matching) { matchingLog = MatchingLogAll; }

            sGui->getIContextSimulator()->enableMatchingMessages(matchingLog);
        }
    }

    void CModelMatcherLogEnable::initGui()
    {
        if (this->hasContexts())
        {
            // avoid signal roundtrips
            const ReverseLookupLogging revLog = sGui->getIContextNetwork()->isReverseLookupMessagesEnabled();
            const bool revLogEnabled = revLog.testFlag(RevLogEnabled);
            if (revLogEnabled != ui->cb_LogReverseLookup->isChecked()) { ui->cb_LogReverseLookup->setChecked(revLogEnabled); }

            const MatchingLog matchingLog = sGui->getIContextSimulator()->isMatchingMessagesEnabled();
            const bool matchingLogEnabled = matchingLog > 0;
            if (matchingLogEnabled != ui->cb_LogMatchingMessages->isChecked()) { ui->cb_LogMatchingMessages->setChecked(matchingLogEnabled); }

            const bool simplified = revLog.testFlag(RevLogSimplifiedInfo) || matchingLog == MatchingLogSimplified;
            const bool detailed = !simplified;
            if (detailed != ui->cb_LogDetailed->isChecked()) { ui->cb_LogDetailed->setChecked(detailed); }
        }
    }

    bool CModelMatcherLogEnable::hasContexts() const
    {
        if (!sGui || sGui->isShuttingDown()) { return false; }
        return sGui->getIContextSimulator() && sGui->getIContextNetwork();
    }

    void CModelMatcherLogEnable::valuesChanged()
    {
        this->initGui();
    }

    void CModelMatcherLogEnable::connectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from);
        if (to.isConnected() || to.isDisconnected())
        {
            this->initGui();
        }
    }
} // ns
