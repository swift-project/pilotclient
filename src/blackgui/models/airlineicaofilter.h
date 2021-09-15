/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRLINEICAOFILTER_H
#define BLACKGUI_AIRLINEICAOFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include <QString>

namespace BlackGui::Models
{
    //! Filter for airline ICAO data
    class BLACKGUI_EXPORT CAirlineIcaoFilter : public IModelFilter<BlackMisc::Aviation::CAirlineIcaoCodeList>
    {
    public:
        //! Constructor
        CAirlineIcaoFilter(int id, const QString &vDesignator, const QString &name,
                            const QString &countryIso, bool isReal, bool isVa);

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::Aviation::CAirlineIcaoCodeList filter(const BlackMisc::Aviation::CAirlineIcaoCodeList &inContainer) const override;

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
