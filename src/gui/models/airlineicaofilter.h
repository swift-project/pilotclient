// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AIRLINEICAOFILTER_H
#define SWIFT_GUI_AIRLINEICAOFILTER_H

#include "gui/swiftguiexport.h"
#include "gui/models/modelfilter.h"
#include "misc/aviation/airlineicaocodelist.h"
#include <QString>

namespace swift::gui::models
{
    //! Filter for airline ICAO data
    class SWIFT_GUI_EXPORT CAirlineIcaoFilter : public IModelFilter<swift::misc::aviation::CAirlineIcaoCodeList>
    {
    public:
        //! Constructor
        CAirlineIcaoFilter(int id, const QString &vDesignator, const QString &name,
                           const QString &countryIso, bool isReal, bool isVa);

        //! \copydoc IModelFilter::filter
        virtual swift::misc::aviation::CAirlineIcaoCodeList filter(const swift::misc::aviation::CAirlineIcaoCodeList &inContainer) const override;

    private:
        int m_id = -1;
        QString m_vDesignator;
        QString m_name;
        QString m_countryIso;
        bool m_real = false;
        bool m_va = false;
    };
} // namespace

#endif // guard
