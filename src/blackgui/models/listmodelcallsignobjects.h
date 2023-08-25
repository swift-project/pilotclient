// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_LISTMODELCALLSIGNOBJECTS_H
#define BLACKGUI_MODELS_LISTMODELCALLSIGNOBJECTS_H

#include "blackgui/models/listmodelbase.h"
#include "blackmisc/aviation/callsignset.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QVariant>
#include <Qt>

class QModelIndex;
class QObject;

namespace BlackGui::Models
{
    //! List model for callsign based objects (callsign is unique key)
    template <typename T, bool UseCompare = false>
    class CListModelCallsignObjects :
        public CListModelBase<T, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! Destructor
        virtual ~CListModelCallsignObjects() {}

        //! Keys to be highlighted
        void setHighlightedCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) { m_highlightCallsigns = callsigns; }

        //! Clear the highlighted callsign
        void clearHighlightedCallsigns() { m_highlightCallsigns.clear(); }

        //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::clearHighlighting
        virtual void clearHighlighting() override
        {
            this->clearHighlightedCallsigns();
            CListModelBase<ContainerType, UseCompare>::clearHighlighting();
        }

        //! Set color for highlighting
        void setHighlightColor(QColor color) { m_highlightColor = color; }

        //! Get data for index and role
        virtual QVariant data(const QModelIndex &index, int role) const override;

        //! Callsign for given index
        BlackMisc::Aviation::CCallsign callsignForIndex(const QModelIndex &index) const;

        //! Highlight index?
        bool isHighlightedIndex(const QModelIndex &index) const;

    protected:
        //! Constructor
        CListModelCallsignObjects(const QString &translationContext, QObject *parent = nullptr);

    private:
        BlackMisc::Aviation::CCallsignSet m_highlightCallsigns; //!< callsigns to be highlighted
        QColor m_highlightColor = Qt::green;
    };
} // namespace
#endif // guard
