/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DATAMAININFOAREACOMPONENT_H
#define BLACKGUI_DATAMAININFOAREACOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/infoarea.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QMainWindow>
#include <QScopedPointer>

namespace Ui { class CDataMainInfoAreaComponent; }

namespace BlackGui
{
    namespace Components
    {
        class CLogComponent;
        class CDbMappingComponent;
        class CDbStashComponent;
        class CDataInfoAreaComponent;

        /**
         * Main info area for data entry tool
         */
        class BLACKGUI_EXPORT CDataMainInfoAreaComponent :
            public BlackGui::CInfoArea,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Info areas
            enum InfoArea
            {
                // index must match tab index!
                InfoAreaData          = 0,
                InfoAreaMapping       = 1,
                InfoAreaStash         = 2,
                InfoAreaSettings      = 3,
                InfoAreaLog           = 4,
                InfoAreaNone          = -1
            };

            //! Constructor
            explicit CDataMainInfoAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDataMainInfoAreaComponent();

            //! Log component
            CLogComponent *getLogComponent() const;

            //! Mapping component
            CDbMappingComponent *getMappingComponent() const;

            //! Mapping component
            CDataInfoAreaComponent *getDataInfoAreaComponent() const;

            //! Stash component
            CDbStashComponent *getStashComponent() const;

            //! Set data reader
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *provider) override;

        protected:
            //! \copydoc CInfoArea::getPreferredSizeWhenFloating
            virtual QSize getPreferredSizeWhenFloating(int areaIndex) const override;

            //! \copydoc CInfoArea::indexToPixmap
            virtual const QPixmap &indexToPixmap(int areaIndex) const override;

        private:
            QScopedPointer <Ui::CDataMainInfoAreaComponent> ui;
        };

    } // ns
} // ns

#endif // guard
