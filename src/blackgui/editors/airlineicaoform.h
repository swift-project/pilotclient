/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_AIRLINEICAOFORM_H
#define BLACKGUI_EDITORS_AIRLINEICAOFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CAirlineIcaoForm; }
namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Airline ICAO code form
         */
        class BLACKGUI_EXPORT CAirlineIcaoForm : public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAirlineIcaoForm(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAirlineIcaoForm();

            //! Set value
            void setValue(const BlackMisc::Aviation::CAirlineIcaoCode &icao = BlackMisc::Aviation::CAirlineIcaoCode());

            //! Get value
            BlackMisc::Aviation::CAirlineIcaoCode getValue() const;

            //! Allow to drop
            void allowDrop(bool allowDrop);

            //! Is drop allowed?
            bool isDropAllowed() const;

            //! \name Form class implementations
            //! @{
            virtual void setReadOnly(bool readonly) override;
            virtual void setSelectOnly() override;
            virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
            //! @}

            //! Clear
            void clear();

            //! Reset value to current value
            void resetValue();

        signals:
            //! Airline has been changed
            void airlineChanged(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

        protected:
            //! \copydoc CForm::jsonPasted
            virtual void jsonPasted(const QString &json) override;

            //! Variant has been dropped
            void onDroppedCode(const BlackMisc::CVariant &variantDropped);

            //! Id entered
            void onIdEntered();

        private:
            QScopedPointer<Ui::CAirlineIcaoForm>  ui;
            BlackMisc::Aviation::CAirlineIcaoCode m_originalCode; //!< object allowing to override values
        };
    } // ns
} //ns

#endif // guard
