// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_NAMEVARIANTPAIRVIEW_H
#define BLACKGUI_NAMEVARIANTPAIRVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/namevariantpairlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "misc/icon.h"
#include "misc/namevariantpairlist.h"
#include "misc/variant.h"

#include <QString>

namespace swift::misc
{
    class CNameVariantPair;
}
namespace BlackGui::Views
{
    //! User view
    class BLACKGUI_EXPORT CNameVariantPairView : public CViewBase<Models::CNameVariantPairModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CNameVariantPairView(QWidget *parent = nullptr);

        //! Icon mode
        void setIconMode(bool withIcon);

        //! Update or add value, QVariant version
        bool addOrUpdateByName(const QString &name, const swift::misc::CVariant &value, const swift::misc::CIcon &icon = swift::misc::CIcon(), bool isResizeConditionMet = true, bool skipEqualValues = true);

        //! Remove by name
        void removeByName(const QString &name, bool isResizeConditionMet = true);

        //! Contains name
        bool containsName(const QString &name);
    };
}
#endif // guard
