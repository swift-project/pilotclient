/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSMATCHINGCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace BlackCore { namespace Context { class IContextSimulator; }}
namespace Ui { class CSettingsMatchingComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Settings for matching component
        class CSettingsMatchingComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsMatchingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CSettingsMatchingComponent() override;

        private:
            QScopedPointer<Ui::CSettingsMatchingComponent> ui;

            //! Save pressed
            void onSavePressed() const;

            //! Reload pressed
            void onReloadPressed();

            //! Do re-matching
            void onMatchingsAgainPressed();

            //! Setup has been changed
            void onSetupChanged();

            //! Deferred reload
            void deferredReload(int deferMs);

            //! Network context
            static BlackCore::Context::IContextSimulator *simulatorContext();
        };
    } // ns
} // ns

#endif // guard
