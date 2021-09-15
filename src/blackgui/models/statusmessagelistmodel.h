/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_STATUSMESSAGELISTMODEL_H
#define BLACKGUI_MODELS_STATUSMESSAGELISTMODEL_H

#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackGui::Models
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
            DetailedWithOrder,
            Simplified,
            SimplifiedWithOrder
        };

        //! Constructor
        explicit CStatusMessageListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CStatusMessageListModel() override {}

        //! Set mode
        void setMode(Mode mode);

        //! Set mode and adjust order/no order
        void setMode(Mode mode, const BlackMisc::CStatusMessageList &messages);

        //! Adjust order column depending on messages
        void adjustOrderColumn(const BlackMisc::CStatusMessageList &messages);

        //! \copydoc QAbstractItemModel::data
        virtual QVariant data(const QModelIndex &index, int role) const override;

        //! Currently sorted by timestamp or by order
        bool isSortedByTimestampOrOrder() const;

        //! Sorted by timestamp or order
        static bool sortedByTimestampOrOrder(const BlackMisc::CPropertyIndex &p);

    private:
        Mode m_mode; //!< used mode
    };
} // ns
#endif // guard
