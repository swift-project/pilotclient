// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_COUNTRYFILTER_H
#define SWIFT_GUI_MODELS_COUNTRYFILTER_H

#include <QString>

#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"
#include "misc/countrylist.h"

namespace swift::gui::models
{
    //! Country filter
    class SWIFT_GUI_EXPORT CCountryFilter : public IModelFilter<swift::misc::CCountryList>
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
} // namespace swift::gui::models

#endif // guard
