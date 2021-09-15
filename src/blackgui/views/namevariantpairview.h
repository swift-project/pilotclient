/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_NAMEVARIANTPAIRVIEW_H
#define BLACKGUI_NAMEVARIANTPAIRVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/namevariantpairlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/icon.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/variant.h"

#include <QString>

namespace BlackMisc { class CNameVariantPair; }
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
        bool addOrUpdateByName(const QString &name, const BlackMisc::CVariant &value, const BlackMisc::CIcon &icon = BlackMisc::CIcon(), bool isResizeConditionMet = true, bool skipEqualValues = true);

        //! Remove by name
        void removeByName(const QString &name, bool isResizeConditionMet = true);

        //! Contains name
        bool containsName(const QString &name);
    };
}
#endif // guard
