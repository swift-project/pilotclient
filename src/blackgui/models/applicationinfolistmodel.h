/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_APPLICATIONINFOLISTMODEL_H
#define BLACKGUI_MODELS_APPLICATIONINFOLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/applicationinfolist.h"
#include <QObject>

class QStandardItemModel;

namespace BlackGui::Models
{
    //! Application info list model
    class BLACKGUI_EXPORT CApplicationInfoListModel : public CListModelBase<BlackMisc::CApplicationInfoList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CApplicationInfoListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CApplicationInfoListModel() {}

        //! \copydoc BlackMisc::CApplicationInfoList::fromOtherSwiftVersionsFromDataDirectories
        void otherSwiftVersionsFromDataDirectories(bool reinit);
    };
} // ns
#endif // guard
