// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_NAMEVARIANTPAIRVIEW_H
#define SWIFT_GUI_NAMEVARIANTPAIRVIEW_H

#include <QString>

#include "gui/models/namevariantpairlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"
#include "misc/icon.h"
#include "misc/variant.h"

namespace swift::misc
{
    class CNameVariantPair;
}
namespace swift::gui::views
{
    //! User view
    class SWIFT_GUI_EXPORT CNameVariantPairView : public CViewBase<models::CNameVariantPairModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CNameVariantPairView(QWidget *parent = nullptr);

        //! Icon mode
        void setIconMode(bool withIcon);

        //! Update or add value, QVariant version
        bool addOrUpdateByName(const QString &name, const swift::misc::CVariant &value,
                               const swift::misc::CIcon &icon = swift::misc::CIcon(), bool isResizeConditionMet = true,
                               bool skipEqualValues = true);

        //! Remove by name
        void removeByName(const QString &name, bool isResizeConditionMet = true);

        //! Contains name
        bool containsName(const QString &name);
    };
} // namespace swift::gui::views
#endif // SWIFT_GUI_NAMEVARIANTPAIRVIEW_H
