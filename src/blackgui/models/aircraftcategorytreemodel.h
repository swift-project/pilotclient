// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H

#include "blackgui/models/columns.h"
#include "misc/aviation/aircraftcategorylist.h"
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
        void updateContainer(const swift::misc::aviation::CAircraftCategoryList &categories);

        //! Clear everything
        //! \remark hiding QStandardItemModel::clear()
        void clear();

        //! Get container
        const swift::misc::aviation::CAircraftCategoryList &container() const { return m_categories; }

    private:
        CColumns m_columns { "CAircraftCategoryTreeModel" };
        swift::misc::aviation::CAircraftCategoryList m_categories;
    };
} // ns

#endif // guard
