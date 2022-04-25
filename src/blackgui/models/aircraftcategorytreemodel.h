/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H

#include "blackgui/models/columns.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackgui/blackguiexport.h"

#include <QStandardItemModel>

namespace BlackGui::Models
{
    //! ATC list model
    class BLACKGUI_EXPORT CAircraftCategoryTreeModel : public QStandardItemModel
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftCategoryTreeModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftCategoryTreeModel() override {}

        //! Set columns
        void setColumns(const CColumns &columns) { m_columns.setColumns(columns); }

        //! Update container
        void updateContainer(const BlackMisc::Aviation::CAircraftCategoryList &categories);

        //! Clear everything
        //! \remark hiding QStandardItemModel::clear()
        void clear();

        //! Get container
        const BlackMisc::Aviation::CAircraftCategoryList &container() const { return m_categories; }

    private:
        CColumns m_columns { "CAircraftCategoryTreeModel" };
        BlackMisc::Aviation::CAircraftCategoryList m_categories;
    };
} // ns

#endif // guard
