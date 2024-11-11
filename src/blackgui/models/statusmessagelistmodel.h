// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_STATUSMESSAGELISTMODEL_H
#define BLACKGUI_MODELS_STATUSMESSAGELISTMODEL_H

#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackgui/blackguiexport.h"
#include "misc/statusmessagelist.h"

namespace BlackGui::Models
{
    /*!
     * Status message list model
     */
    class BLACKGUI_EXPORT CStatusMessageListModel :
        public CListModelTimestampObjects<swift::misc::CStatusMessageList, true>
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
        void setMode(Mode mode, const swift::misc::CStatusMessageList &messages);

        //! Adjust order column depending on messages
        void adjustOrderColumn(const swift::misc::CStatusMessageList &messages);

        //! \copydoc QAbstractItemModel::data
        virtual QVariant data(const QModelIndex &index, int role) const override;

        //! Currently sorted by timestamp or by order
        bool isSortedByTimestampOrOrder() const;

        //! Sorted by timestamp or order
        static bool sortedByTimestampOrOrder(const swift::misc::CPropertyIndex &p);

    private:
        Mode m_mode; //!< used mode
    };
} // ns
#endif // guard
