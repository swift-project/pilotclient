/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_AIRCRAFTICAOFORM_H
#define BLACKGUI_EDITORS_AIRCRAFTICAOFORM_H

#include "blackgui/editors/form.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include "blackmisc/statusmessagelist.h"
#include <QScopedPointer>

namespace Ui { class CAircraftIcaoForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Aircraft ICAO form
         */
        class CAircraftIcaoForm :
            public CForm,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftIcaoForm(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftIcaoForm();

            //! Get value
            BlackMisc::Aviation::CAircraftIcaoCode getValue() const;

            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validate() const;

            //! Allow to drop
            void allowDrop(bool allowDrop);

            //! Is drop allowed?
            bool isDropAllowed() const;

            //! \copydoc CForm::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

            //! Clear
            void clear();

            //! Mapping mode
            void setMappingMode();

            //! \copydoc CWebDataServicesAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

        public slots:
            //! Set value
            void setValue(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

        private slots:
            //! Variant has been dropped
            void ps_droppedCode(const BlackMisc::CVariant &variantDropped);

        private:
            QScopedPointer<Ui::CAircraftIcaoForm> ui;

            //! Key from GUI
            int getDbKeyFromGui() const;
        };
    } // ns
} // ns

#endif // guard
