// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_TEXTMESSAGELISTMODEL_H
#define SWIFT_GUI_MODELS_TEXTMESSAGELISTMODEL_H

#include "gui/models/listmodeltimestampobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/network/textmessagelist.h"

class QObject;

namespace swift::misc::network
{
    class CTextMessage;
}
namespace swift::gui::models
{
    //! Text message list model
    class SWIFT_GUI_EXPORT CTextMessageListModel :
        public CListModelTimestampObjects<swift::misc::network::CTextMessageList, true>
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
        ~CTextMessageListModel() override = default;

        //! Set mode
        void setTextMessageMode(TextMessageMode mode);

        //! Mode
        TextMessageMode getTextMessageMode() const { return m_textMessageMode; }

        //! \copydoc QAbstractItemModel::data
        QVariant data(const QModelIndex &index, int role) const override;

    private:
        TextMessageMode m_textMessageMode = NotSet;
    };
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_TEXTMESSAGELISTMODEL_H
