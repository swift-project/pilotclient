// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_TEXTMESSAGELISTMODEL_H
#define BLACKGUI_MODELS_TEXTMESSAGELISTMODEL_H

#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/textmessagelist.h"

class QObject;

namespace BlackMisc::Network
{
    class CTextMessage;
}
namespace BlackGui::Models
{
    //! Text message list model
    class BLACKGUI_EXPORT CTextMessageListModel :
        public CListModelTimestampObjects<BlackMisc::Network::CTextMessageList, true>
    {
        Q_OBJECT

    public:
        //! What kind of stations
        enum TextMessageMode
        {
            NotSet,
            FromTo,
            From
        };

        //! Constructor
        explicit CTextMessageListModel(TextMessageMode stationMode, QObject *parent = nullptr);

        //! Destructor
        virtual ~CTextMessageListModel() override {}

        //! Set mode
        void setTextMessageMode(TextMessageMode mode);

        //! Mode
        TextMessageMode getTextMessageMode() const { return m_textMessageMode; }

        //! \copydoc QAbstractItemModel::data
        virtual QVariant data(const QModelIndex &index, int role) const override;

    private:
        TextMessageMode m_textMessageMode = NotSet;
    };
} // ns

#endif // guard
