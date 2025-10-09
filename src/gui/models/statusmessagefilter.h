// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_STATUSMESSAGEFILTER_H
#define SWIFT_GUI_MODELS_STATUSMESSAGEFILTER_H

#include <QString>

#include "gui/models/modelfilter.h"
#include "gui/swiftguiexport.h"
#include "misc/statusmessagelist.h"

namespace swift::misc
{
    class CLogPattern;
}

namespace swift::gui::models
{
    //! Filter for status messages
    class SWIFT_GUI_EXPORT CStatusMessageFilter : public IModelFilter<swift::misc::CStatusMessageList>
    {
    public:
        //! Constructor
        CStatusMessageFilter(swift::misc::CStatusMessage::StatusSeverity severity, const QString &text,
                             const QString &category);

        //! \copydoc IModelFilter::filter
        swift::misc::CStatusMessageList filter(const swift::misc::CStatusMessageList &inContainer) const override;

        //! \copydoc IModelFilter::getAsValueObject
        swift::misc::CVariant getAsValueObject() const override;

    private:
        swift::misc::CStatusMessage::StatusSeverity m_severity = swift::misc::CStatusMessage::SeverityError;
        QString m_msgText;
        QString m_category;
    };
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_STATUSMESSAGEFILTER_H
