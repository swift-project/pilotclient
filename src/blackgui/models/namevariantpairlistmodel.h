// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_NAMEVARIANTLISTMODEL_H
#define BLACKGUI_NAMEVARIANTLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/variant.h"

#include <QString>

class QObject;

namespace BlackMisc
{
    class CIcon;
    class CNameVariantPair;
}

namespace BlackGui::Models
{

    //! Simple model displaying name / variant values
    class BLACKGUI_EXPORT CNameVariantPairModel : public CListModelBase<BlackMisc::CNameVariantPairList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CNameVariantPairModel(bool withIcon, QObject *parent = nullptr);

        //! Destructor
        virtual ~CNameVariantPairModel() {}

        //! Icon on / off
        void setIconMode(bool withIcon);

        //! Remove by given name
        void removeByName(const QString &name);

        //! Contains name already?
        bool containsName(const QString &name) const;

        //! Contains name / value?
        bool containsNameValue(const QString &name, const BlackMisc::CVariant &value) const;

        //! Add our update a value
        bool addOrUpdateByName(const QString &name, const BlackMisc::CVariant &value, const BlackMisc::CIcon &icon, bool skipEqualValues);

        //! Current row index of given name
        int getRowIndexForName(const QString &name) const;
    };
}
#endif // guard
