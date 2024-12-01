// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H

#include <QStandardItemModel>

#include "gui/models/columns.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftcategorylist.h"

namespace swift::gui::models
{
    //! ATC list model
    class SWIFT_GUI_EXPORT CAircraftCategoryTreeModel : public QStandardItemModel
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
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_AIRCRAFTCATEGORYTREEMODEL_H
