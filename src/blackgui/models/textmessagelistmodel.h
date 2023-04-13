/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
