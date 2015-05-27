/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ORIGINATORLISTMODEL_H
#define BLACKGUI_ORIGINATORLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/originatorlist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {
        //! Originator list model
        class BLACKGUI_EXPORT COriginatorListModel : public CListModelBase<BlackMisc::COriginator, BlackMisc::COriginatorList>
        {
        public:

            //! Constructor
            explicit COriginatorListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~COriginatorListModel() {}
        };
    } // ns
} // ns
#endif // guard
