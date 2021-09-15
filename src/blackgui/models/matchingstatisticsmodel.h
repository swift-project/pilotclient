/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MATCHINGSTATISTICSMODEL_H
#define BLACKGUI_MATCHINGSTATISTICSMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/simulation/matchingstatistics.h"

class QObject;

namespace BlackGui::Models
{
    //! Matching statistics entry
    class BLACKGUI_EXPORT CMatchingStatisticsModel : public CListModelBase<BlackMisc::Simulation::CMatchingStatistics, true>
    {
        Q_OBJECT

    public:
        //! How to display
        enum MatchingStatisticsMode
        {
            NoSet,
            ForSingleSession,
            ForMultiSessions
        };

        //! Constructor
        explicit CMatchingStatisticsModel(MatchingStatisticsMode mode, QObject *parent = nullptr);

        //! Destructor
        virtual ~CMatchingStatisticsModel() {}

        //! Set mode
        void setMode(MatchingStatisticsMode mode);

    private:
        MatchingStatisticsMode m_mode = NoSet;
    };
} // ns
#endif // guard
