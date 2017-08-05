/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDMONITORDIALOG_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDMONITORDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/logcategorylist.h"

namespace Ui { class CSimulatorEmulatedMonitorDialog; }
namespace BlackSimPlugin
{
    namespace Emulated
    {
        class CSimulatorEmulated;

        /**
         * Monitor widget for the pseudo driver
         */
        class CSimulatorEmulatedMonitorDialog : public QDialog
        {
            Q_OBJECT

        public:
            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Ctor
            explicit CSimulatorEmulatedMonitorDialog(CSimulatorEmulated *simulator, QWidget *parent = nullptr);

            //! Dtor
            virtual ~CSimulatorEmulatedMonitorDialog();

            //! \copydoc BlackGui::Components::CLogComponent::appendStatusMessageToList
            void appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage);

            //! \copydoc BlackGui::Components::CLogComponent::appendStatusMessagesToList
            void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages);

            //! Append a function call as status message
            void appendFunctionCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        private:
            //! UI values changed
            void onSimulatorValuesChanged();

            //! Update aircraft situation
            void changeSituation();

            //! Change the aircraft parts
            void changeParts();

            //! UI values
            void setSimulatorUiValues();

            QScopedPointer<Ui::CSimulatorEmulatedMonitorDialog> ui;
            CSimulatorEmulated *m_simulator = nullptr;
        };
    } // ns
} // ns

#endif // guard
