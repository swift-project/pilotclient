/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_LIVERYFORM_H
#define BLACKGUI_EDITORS_LIVERYFORM_H

#include "blackgui/editors/form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/statusmessagelist.h"

namespace Ui { class CLiveryForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Livery form class
         */
        class BLACKGUI_EXPORT CLiveryForm :
            public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLiveryForm(QWidget *parent = nullptr);

            //! Destructor
            ~CLiveryForm();

            //! Value
            BlackMisc::Aviation::CLivery getValue() const;

            //! Embedded ariline
            BlackMisc::Aviation::CAirlineIcaoCode getValueAirlineIcao() const;

            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validate(bool withNestedForms) const;

            //! Validate airline ICAO code only
            BlackMisc::CStatusMessageList validateAirlineIcao() const;

            //! Allow to drop
            void allowDrop(bool allowDrop);

            //! Is drop allowed?
            bool isDropAllowed() const;

            //! \copydoc CForm::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

            //! \copydoc CForm::setSelectOnly
            virtual void setSelectOnly() override;

            //! Clear data
            void clear();

        public slots:
            //! Value
            void setValue(const BlackMisc::Aviation::CLivery &livery);

        private slots:
            //! Livery dropped
            void ps_droppedLivery(const BlackMisc::CVariant &variantDropped);

            //! Airline of embedded form has changed
            void ps_airlineChanged(const BlackMisc::Aviation::CAirlineIcaoCode &code);

        private:
            QScopedPointer<Ui::CLiveryForm> ui;
        };
    } // ns
} // ns

#endif // guard
