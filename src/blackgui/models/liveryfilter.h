/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LIVERYFILTER_H
#define BLACKGUI_LIVERYFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/rgbcolor.h"

#include <QString>

namespace BlackGui::Models
{
    //! Filter for aircraft liveries
    class BLACKGUI_EXPORT CLiveryFilter : public IModelFilter<BlackMisc::Aviation::CLiveryList>
    {
    public:
        //! Constructor
        CLiveryFilter(
            int id,
            const QString &combinedCode,
            const QString &descriptiom,
            const QString &airlineDesignator,
            const BlackMisc::CRgbColor &fuselageColor,
            const BlackMisc::CRgbColor &tailColor,
            double maxColorDistance,
            bool colorLiveries,
            bool airlineLiveries
        );

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::Aviation::CLiveryList filter(const BlackMisc::Aviation::CLiveryList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_combinedCode;
        QString m_description;
        QString m_airlineIcaoDesignator;
        BlackMisc::CRgbColor m_fuselageColor;
        BlackMisc::CRgbColor m_tailColor;
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
