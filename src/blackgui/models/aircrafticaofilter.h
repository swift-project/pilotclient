// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_AIRCRAFTICAOFILTER_H
#define BLACKGUI_AIRCRAFTICAOFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "misc/aviation/aircrafticaocodelist.h"

#include <QString>

namespace BlackGui::Models
{
    //! Filter for aircraft ICAO data
    class BLACKGUI_EXPORT CAircraftIcaoFilter :
        public IModelFilter<swift::misc::aviation::CAircraftIcaoCodeList>
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
        virtual swift::misc::aviation::CAircraftIcaoCodeList filter(const swift::misc::aviation::CAircraftIcaoCodeList &inContainer) const override;

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
