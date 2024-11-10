// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_STATUSMESSAGEFILTER_H
#define BLACKGUI_MODELS_STATUSMESSAGEFILTER_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/modelfilter.h"
#include "misc/statusmessagelist.h"
#include <QString>

namespace swift::misc
{
    class CLogPattern;
}

namespace BlackGui::Models
{
    //! Filter for status messages
    class BLACKGUI_EXPORT CStatusMessageFilter : public IModelFilter<swift::misc::CStatusMessageList>
    {
    public:
        //! Constructor
        CStatusMessageFilter(swift::misc::CStatusMessage::StatusSeverity severity, const QString &text, const QString &category);

        //! \copydoc IModelFilter::filter
        virtual swift::misc::CStatusMessageList filter(const swift::misc::CStatusMessageList &inContainer) const override;

        //! \copydoc IModelFilter::getAsValueObject
        virtual swift::misc::CVariant getAsValueObject() const override;

    private:
        swift::misc::CStatusMessage::StatusSeverity m_severity = swift::misc::CStatusMessage::SeverityError;
        QString m_msgText;
        QString m_category;
    };
} // namespace

#endif // guard
