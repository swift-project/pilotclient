/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_NAMEVARIANTPAIRVIEW_H
#define BLACKGUI_NAMEVARIANTPAIRVIEW_H

#include "viewbase.h"
#include "../models/namevariantpairlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! User view
        class CNameVariantPairView : public CViewBase<Models::CNameVariantPairModel>
        {

        public:
            //! Constructor
            explicit CNameVariantPairView(QWidget *parent = nullptr);

            //! Icon mode
            void setIconMode(bool withIcon);

            //! Update or add value, simple string version
            void addOrUpdateByName(const QString &name, const QString &value, const BlackMisc::CIcon &icon = BlackMisc::CIcon());

            //! Remove by name
            void removeByName(const QString &name);
        };
    }
}
#endif // guard
