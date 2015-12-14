/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRLINENAMESELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "dbairlineicaoselectorbase.h"
#include <QScopedPointer>

namespace Ui { class CDbAirlineNameSelectorComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Select airline by name
         */
        class BLACKGUI_EXPORT CDbAirlineNameSelectorComponent : public CDbAirlineIcaoSelectorBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAirlineNameSelectorComponent(QWidget *parent = nullptr);

            //! Destructor
            //! \note needed for forward declared QScopedPointer and needs to be in .cpp
            virtual ~CDbAirlineNameSelectorComponent();

            //! \copydoc CDbAirlineIcaoSelectorBase::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

            //! \copydoc CDbAirlineIcaoSelectorBase::setAirlineIcao
            virtual void setAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao) override;

            //! \copydoc CDbAirlineIcaoSelectorBase::clear
            virtual void clear() override;

        protected:
            //! \copydoc CDbAirlineIcaoSelectorBase::createCompleter
            virtual QCompleter *createCompleter() override;

        private:
            QScopedPointer<Ui::CDbAirlineNameSelectorComponent> ui;

        private slots:
            //! Data changed
            void ps_dataChanged();
        };
    } // ns
} // ns

#endif // guard
