// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_SELECTIONMODEL_H
#define SWIFT_GUI_MODELS_SELECTIONMODEL_H

namespace swift::gui::models
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
} // namespace swift::gui::models
#endif // guard
