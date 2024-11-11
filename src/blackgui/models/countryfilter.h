// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_COUNTRYFILTER_H
#define BLACKGUI_MODELS_COUNTRYFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "misc/countrylist.h"

#include <QString>

namespace BlackGui::Models
{
    //! Country filter
    class BLACKGUI_EXPORT CCountryFilter : public IModelFilter<swift::misc::CCountryList>
    {
    public:
        //! Constructor
        CCountryFilter(const QString &isoCode, const QString &name);

        //! \copydoc IModelFilter::filter
        virtual swift::misc::CCountryList filter(const swift::misc::CCountryList &inContainer) const override;

    private:
        QString m_isoCode;
        QString m_name;
    };
} // namespace

#endif // guard
