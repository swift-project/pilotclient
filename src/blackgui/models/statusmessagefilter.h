// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_STATUSMESSAGEFILTER_H
#define BLACKGUI_MODELS_STATUSMESSAGEFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/statusmessagelist.h"
#include <QString>

namespace BlackMisc
{
    class CLogPattern;
}

namespace BlackGui::Models
{
    //! Filter for status messages
    class BLACKGUI_EXPORT CStatusMessageFilter : public IModelFilter<BlackMisc::CStatusMessageList>
    {
    public:
        //! Constructor
        CStatusMessageFilter(BlackMisc::CStatusMessage::StatusSeverity severity, const QString &text, const QString &category);

        //! \copydoc IModelFilter::filter
        virtual BlackMisc::CStatusMessageList filter(const BlackMisc::CStatusMessageList &inContainer) const override;

        //! \copydoc IModelFilter::getAsValueObject
        virtual BlackMisc::CVariant getAsValueObject() const override;

    private:
        BlackMisc::CStatusMessage::StatusSeverity m_severity = BlackMisc::CStatusMessage::SeverityError;
        QString m_msgText;
        QString m_category;
    };
} // namespace

#endif // guard
