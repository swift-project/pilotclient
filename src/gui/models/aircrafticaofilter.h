// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AIRCRAFTICAOFILTER_H
#define SWIFT_GUI_AIRCRAFTICAOFILTER_H

#include <QString>

#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircrafticaocodelist.h"

namespace swift::gui::models
{
    //! Filter for aircraft ICAO data
    class SWIFT_GUI_EXPORT CAircraftIcaoFilter : public IModelFilter<swift::misc::aviation::CAircraftIcaoCodeList>
    {
    public:
        //! Constructor
        CAircraftIcaoFilter(int id, const QString &designator, const QString &family, const QString &manufacturer,
                            const QString &description, const QString &combinedType);

        //! \copydoc IModelFilter::filter
        virtual swift::misc::aviation::CAircraftIcaoCodeList
        filter(const swift::misc::aviation::CAircraftIcaoCodeList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_designator;
        QString m_family;
        QString m_manufacturer;
        QString m_description;
        QString m_combinedType;
    };
} // namespace swift::gui::models

#endif // guard
