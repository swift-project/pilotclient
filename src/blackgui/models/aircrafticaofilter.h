/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTICAOFILTER_H
#define BLACKGUI_AIRCRAFTICAOFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"

#include <QString>

namespace BlackGui::Models
{
    //! Filter for aircraft ICAO data
    class BLACKGUI_EXPORT CAircraftIcaoFilter :
        public IModelFilter<BlackMisc::Aviation::CAircraftIcaoCodeList>
    {
    public:
        //! Constructor
        CAircraftIcaoFilter(int id,
                            const QString &designator,
                            const QString &family,
                            const QString &manufacturer,
                            const QString &description,
                            const QString &combinedType);

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::Aviation::CAircraftIcaoCodeList filter(const BlackMisc::Aviation::CAircraftIcaoCodeList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_designator;
        QString m_family;
        QString m_manufacturer;
        QString m_description;
        QString m_combinedType;
    };
} // namespace

#endif // guard
