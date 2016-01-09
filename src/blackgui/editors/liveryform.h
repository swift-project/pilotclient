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
#include "blackmisc/aviation/livery.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include "blackmisc/statusmessagelist.h"

namespace Ui { class CLiveryForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Livery form class
         */
        class CLiveryForm :
            public CForm,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLiveryForm(QWidget *parent = nullptr);

            //! Destructor
            ~CLiveryForm();

            //! \copydoc CWebDataServicesAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *provider) override;

            //! Value
            BlackMisc::Aviation::CLivery getValue() const;

            //! Value
            void setValue(const BlackMisc::Aviation::CLivery &livery);

            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validate(bool withNestedForms) const;

            //! Allow to drop
            void allowDrop(bool allowDrop);

            //! Is drop allowed?
            bool isDropAllowed() const;

            //! \copydoc CForm::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

            //! Mapping mode
            void setMappingMode(bool mappingMode);

            //! Clear data
            void clear();

        private slots:
            //! Livery dropped
            void ps_droppedLivery(const BlackMisc::CVariant &variantDropped);

        private:
            QScopedPointer<Ui::CLiveryForm> ui;
            BlackMisc::Aviation::CLivery    m_originalLivery; //!< object whose values will be overridden
        };

    } // ns
} // ns

#endif // guard
