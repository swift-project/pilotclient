/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_STATUSMESSAGELISTMODEL_H
#define BLACKGUI_MODELS_STATUSMESSAGELISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackmisc/statusmessagelist.h"

class QObject;

namespace BlackGui
{
    namespace Models
    {
        /*!
         * Status message list model
         */
        class BLACKGUI_EXPORT CStatusMessageListModel :
            public CListModelTimestampObjects<BlackMisc::CStatusMessageList, true>
        {
            Q_OBJECT

        public:
            //! Mode
            enum Mode
            {
                Detailed,
                Simplified
            };

            //! Constructor
            explicit CStatusMessageListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CStatusMessageListModel() override {}

            //! Set mode
            void setMode(Mode mode);

            //! \copydoc QAbstractItemModel::data
            virtual QVariant data(const QModelIndex &index, int role) const override;
        };
    } // ns
} // ns
#endif // guard
