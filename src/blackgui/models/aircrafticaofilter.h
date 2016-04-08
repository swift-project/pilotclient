/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTICAOFILTER_H
#define BLACKGUI_AIRCRAFTICAOFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"

namespace BlackGui
{
    namespace Models
    {

        //! Filter for aircraft ICAO data
        class BLACKGUI_EXPORT CAircraftIcaoFilter :
            public IModelFilter<BlackMisc::Aviation::CAircraftIcaoCodeList>
        {
        public:
            //! Constructor
            CAircraftIcaoFilter(const QString &designator,
                                const QString &manufacturer,
                                const QString &description,
                                const QString &combinedType);

            //! \copydoc IModelFilter::filter
            virtual BlackMisc::Aviation::CAircraftIcaoCodeList filter(const BlackMisc::Aviation::CAircraftIcaoCodeList &inContainer) const override;

        private:
            QString m_designator;
            QString m_manufacturer;
            QString m_description;
            QString m_combinedType;
        };

    } // namespace
} // namespace

#endif // guard
