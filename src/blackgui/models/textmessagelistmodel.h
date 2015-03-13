/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEXTMESSAGELISTMODEL_H
#define BLACKGUI_TEXTMESSAGELISTMODEL_H

#include "blackmisc/nwtextmessagelist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    namespace Models
    {
        //! ATC list model
        class CTextMessageListModel : public CListModelBase<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>
        {

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
            virtual ~CTextMessageListModel() {}

            //! Set mode
            void setTextMessageMode(TextMessageMode mode);

            //! Mode
            TextMessageMode getTextMessageMode() const { return this->m_textMessageMode; }

        private:
            TextMessageMode m_textMessageMode = NotSet;
        };
    }
}
#endif // guard
