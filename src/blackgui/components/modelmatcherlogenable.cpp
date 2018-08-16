/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmatcherlogenable.h"
#include "guiapplication.h"
#include "ui_modelmatcherlogenable.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"

#include <QPointer>
#include <QTimer>

using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CModelMatcherLogEnable::CModelMatcherLogEnable(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CModelMatcherLogEnable)
        {
            ui->setupUi(this);
            connect(ui->cb_LogReverseLookup, &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged);
            connect(ui->cb_LogMatchingMessages, &QCheckBox::toggled, this, &CModelMatcherLogEnable::enabledCheckboxChanged);

            if (this->hasContexts())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::changedLogOrDebugSettings, this, &CModelMatcherLogEnable::valuesChanged, Qt::QueuedConnection);
                connect(sGui->getIContextNetwork(), &IContextNetwork::changedLogOrDebugSettings, this, &CModelMatcherLogEnable::valuesChanged, Qt::QueuedConnection);
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CModelMatcherLogEnable::connectionStatusChanged, Qt::QueuedConnection);
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
            if (sender == ui->cb_LogReverseLookup)
            {
                sGui->getIContextNetwork()->enableReverseLookupMessages(enabled);
            }
            else if (sender == ui->cb_LogMatchingMessages)
            {
                sGui->getIContextSimulator()->enableMatchingMessages(enabled);
            }
        }

        void CModelMatcherLogEnable::initGui()
        {
            if (this->hasContexts())
            {
                // avoid signal roundtrips
                bool c = sGui->getIContextNetwork()->isReverseLookupMessagesEnabled();
                if (c != ui->cb_LogReverseLookup->isChecked())
                {
                    ui->cb_LogReverseLookup->setChecked(c);
                }

                c = sGui->getIContextSimulator()->isMatchingMessagesEnabled();
                if (c != ui->cb_LogMatchingMessages->isChecked())
                {
                    ui->cb_LogMatchingMessages->setChecked(c);
                }
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

        void CModelMatcherLogEnable::connectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (to == INetwork::Connected || to == INetwork::Disconnected)
            {
                this->initGui();
            }
        }
    } // ns
} // ns
