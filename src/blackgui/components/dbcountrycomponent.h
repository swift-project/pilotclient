/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBCOUNTRYCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBCOUNTRYCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackmisc/network/entityflags.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CDbCountryComponent; }
namespace BlackGui
{
    namespace Components
    {
        /*!
         * The countries
         */
        class BLACKGUI_EXPORT CDbCountryComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbCountryComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbCountryComponent();

        private:
            //! Countries have been read
            void onCountriesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Reload models
            void onReload();

        private:
            QScopedPointer<Ui::CDbCountryComponent> ui;
        };

    } // ns
} // ns

#endif // guard
