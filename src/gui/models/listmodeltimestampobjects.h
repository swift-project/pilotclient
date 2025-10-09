// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_LISTMODELTIMESTAMPOBJECTS_H
#define SWIFT_GUI_MODELS_LISTMODELTIMESTAMPOBJECTS_H

#include <QString>

#include "gui/models/listmodelbase.h"

class QModelIndex;
class QObject;

namespace swift::gui::models
{
    //! List model for timestamp based objects with offset
    template <typename T, bool UseCompare = false>
    class CListModelTimestampObjects : public CListModelBase<T, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! Sorted by one of the timestamp columns?
        bool isSortedByTimestampProperty() const;

    protected:
        //! Constructor
        CListModelTimestampObjects(const QString &translationContext, QObject *parent = nullptr);

        //! Standard timestamp columns
        void addTimestampColumns();
    };

    //! List model for timestamp based objects with offset
    template <typename T, bool UseCompare = false>
    class CListModelTimestampWithOffsetObjects : public CListModelTimestampObjects<T, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! Destructor
        ~CListModelTimestampWithOffsetObjects() override = default;

        //! Insert as first element by keeping maxElements and the latest first
        void push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max);

    protected:
        //! Constructor
        CListModelTimestampWithOffsetObjects(const QString &translationContext, QObject *parent = nullptr);

        //! Standard timestamp offset columns
        void addTimestampOffsetColumns();
    };
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_LISTMODELTIMESTAMPOBJECTS_H
