/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "modelmatcherlogenable.h"
#include "blackgui/guiapplication.h"
#include "ui_modelmatcherlogenable.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"

#include <QPointer>
#include <QTimer>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CModelMatcherLogEnable::CModelMatcherLogEnable(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CModelMatcherLogEnable)
    {
        ui->setupUi(this);
        connect(ui->cb_LogReverseLookup,    &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged, Qt::QueuedConnection);
        connect(ui->cb_LogMatchingMessages, &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged, Qt::QueuedConnection);
        connect(ui->cb_LogDetailed,         &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged, Qt::QueuedConnection);

        if (this->hasContexts())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::changedLogOrDebugSettings, this, &CModelMatcherLogEnable::valuesChanged, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(),   &IContextNetwork::changedLogOrDebugSettings,   this, &CModelMatcherLogEnable::valuesChanged, Qt::QueuedConnection);
            connect(sGui->getIContextNetwork(),   &IContextNetwork::connectionStatusChanged,     this, &CModelMatcherLogEnable::connectionStatusChanged, Qt::QueuedConnection);
        }

        QPointer<CModelMatcherLogEnable> myself(this);
        QTimer::singleShot(5000, this, [ = ]
        {
            if (!myself) { return; }
            this->initGui();
        });
    }

    CModelMatcherLogEnable::~CModelMatcherLogEnable()
    { }

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

        const bool detailed  = (sender == ui->cb_LogDetailed)         ? enabled : ui->cb_LogDetailed->isChecked();
        const bool reverse   = (sender == ui->cb_LogReverseLookup)    ? enabled : ui->cb_LogReverseLookup->isChecked();
        const bool matching  = (sender == ui->cb_LogMatchingMessages) ? enabled : ui->cb_LogMatchingMessages->isChecked();
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
