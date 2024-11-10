// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_LIVERYFILTER_H
#define SWIFT_GUI_LIVERYFILTER_H

#include "gui/swiftguiexport.h"
#include "gui/models/modelfilter.h"
#include "misc/aviation/liverylist.h"
#include "misc/rgbcolor.h"

#include <QString>

namespace swift::gui::models
{
    //! Filter for aircraft liveries
    class SWIFT_GUI_EXPORT CLiveryFilter : public IModelFilter<swift::misc::aviation::CLiveryList>
    {
    public:
        //! Constructor
        CLiveryFilter(
            int id,
            const QString &combinedCode,
            const QString &descriptiom,
            const QString &airlineDesignator,
            const swift::misc::CRgbColor &fuselageColor,
            const swift::misc::CRgbColor &tailColor,
            double maxColorDistance,
            bool colorLiveries,
            bool airlineLiveries);

        //! \copydoc IModelFilter::filter
        virtual swift::misc::aviation::CLiveryList filter(const swift::misc::aviation::CLiveryList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_combinedCode;
        QString m_description;
        QString m_airlineIcaoDesignator;
        swift::misc::CRgbColor m_fuselageColor;
        swift::misc::CRgbColor m_tailColor;
        double m_maxColorDistance = 0.5;
        bool m_colorLiveries = true;
        bool m_airlineLiveries = true;

        //! Filter by livery type such as color or airline liveries
        bool filterByLiveryType() const;

        //! Valid filter?
        bool valid() const;
    };
} // namespace

#endif // guard
