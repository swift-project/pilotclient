/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SWIFT_SIMULATORSWIFTMONITORDIALOG_H
#define BLACKSIMPLUGIN_SWIFT_SIMULATORSWIFTMONITORDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/logcategorylist.h"

namespace Ui { class CSimulatorSwiftMonitorDialog; }
namespace BlackSimPlugin
{
    namespace Swift
    {
        class CSimulatorSwift;

        /**
         * Monitor widget for the pseudo driver
         */
        class CSimulatorSwiftMonitorDialog : public QDialog
        {
            Q_OBJECT

        public:
            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Ctor
            explicit CSimulatorSwiftMonitorDialog(CSimulatorSwift *simulator, QWidget *parent = nullptr);

            //! Dtor
            virtual ~CSimulatorSwiftMonitorDialog();

            //! \copydoc BlackGui::Components::CLogComponent::appendStatusMessageToList
            void appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage);

            //! \copydoc BlackGui::Components::CLogComponent::appendStatusMessagesToList
            void appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages);

            //! Append a function call as status message
            void appendFunctionCall(const QString &function, const QString &p1 = {}, const QString &p2 = {}, const QString &p3 = {});

        private:
            //! UI values changed
            void onSimulatorValuesChanged();

            //! UI values
            void setSimulatorUiValues();

            QScopedPointer<Ui::CSimulatorSwiftMonitorDialog> ui;
            CSimulatorSwift *m_simulator = nullptr;
        };
    } // ns
} // ns

#endif // guard
