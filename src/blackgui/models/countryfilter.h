/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_COUNTRYFILTER_H
#define BLACKGUI_MODELS_COUNTRYFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/countrylist.h"

#include <QString>

namespace BlackGui::Models
{
    //! Country filter
    class BLACKGUI_EXPORT CCountryFilter : public IModelFilter<BlackMisc::CCountryList>
    {
    public:
        //! Constructor
        CCountryFilter(const QString &isoCode, const QString &name);

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::CCountryList filter(const BlackMisc::CCountryList &inContainer) const override;

    private:
        QString m_isoCode;
        QString m_name;
    };
} // namespace

#endif // guard
