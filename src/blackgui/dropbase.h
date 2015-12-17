/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_DROPBASE_H
#define BLACKGUI_DROPBASE_H

#include "blackmisc/variant.h"
#include <QLabel>
#include <QMimeData>

namespace BlackGui
{
    /*!
     * Utilities for dropping swift value objects
     */
    class CDropBase
    {
    public:
        //! Set text for drop site
        void setInfoText(const QString &dropSiteText);

        //! Accepted ids
        void setAcceptedMetaTypeIds(const QList<int> &ids);

        //! Accepted ids
        void addAcceptedMetaTypeId(int id);

        //! Drop allowed
        virtual bool isDropAllowed() const;

        //! Drop allowed
        virtual void allowDrop(bool allowed);

        //! Mime data to CVariant (normally encapsulating a value object)
        BlackMisc::CVariant toCVariant(const QMimeData *mime) const;

    protected:
        CDropBase();

        //! Accept drop
        bool acceptDrop(const QMimeData *mime) const;

    private:
        bool       m_allowDrop = true;   //!< dropping allowed?
        QList<int> m_acceptedMetaTypes;  //!< accepted meta types
    };

} // ns

#endif // guard
