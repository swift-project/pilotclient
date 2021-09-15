/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_SELECTIONMODEL_H
#define BLACKGUI_MODELS_SELECTIONMODEL_H

namespace BlackGui::Models
{
    //! Allow to get and select objects
    template <typename ContainerType> class ISelectionModel
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
