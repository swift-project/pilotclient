// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_SELECTIONMODEL_H
#define BLACKGUI_MODELS_SELECTIONMODEL_H

namespace BlackGui::Models
{
    //! Allow to get and select objects
    template <typename ContainerType>
    class ISelectionModel
    {
    public:
        //! Destructor
        virtual ~ISelectionModel() {}

        //! Selected objects
        virtual ContainerType selectedObjects() const = 0;

        //! Unselected objects
        //! \remark for filtered models this only returns the unselected filtered objects
        virtual ContainerType unselectedObjects() const = 0;

        //! Select
        virtual void selectObjects(const ContainerType &selectedObjects) = 0;
    };
} // namespace
#endif // guard
