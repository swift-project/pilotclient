/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRLINEICAOSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "dbairlineicaoselectorbase.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include <QScopedPointer>

namespace Ui { class CDbAirlineIcaoSelectorComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Airline ICAO selector
         */
        class BLACKGUI_EXPORT CDbAirlineIcaoSelectorComponent : public CDbAirlineIcaoSelectorBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAirlineIcaoSelectorComponent(QWidget *parent = nullptr);

            //! Destructor
            //! \note needed for forward declared QScopedPointer and needs to be in .cpp
            ~CDbAirlineIcaoSelectorComponent();

            //! \name Base class overrides
            //! @{
            virtual void setReadOnly(bool readOnly) override;
            virtual void setAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao) override;
            virtual void clear() override;
            //! @}

            //! Raw text designator
            QString getRawDesignator() const;

            //! Display ICAO description
            void withIcaoDescription(bool description);

        protected:
            //! \copydoc CDbAirlineIcaoSelectorBase::createCompleter
            virtual QCompleter *createCompleter() override;

        private slots:
            //! Data have been changed
            void ps_dataChanged();

        private:
            Ui::CDbAirlineIcaoSelectorComponent *ui;
        };
    }
} // ns
#endif // guard
