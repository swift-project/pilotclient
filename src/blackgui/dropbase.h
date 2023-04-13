/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKGUI_DROPBASE_H
#define BLACKGUI_DROPBASE_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/variant.h"
#include <QList>
#include <QFileInfo>
#include <QMimeData>

namespace BlackGui
{
    /*!
     * Utilities for dropping swift value objects
     */
    class BLACKGUI_EXPORT CDropBase
    {
    public:
        //! Dtor
        virtual ~CDropBase();

        //! Accepted ids
        void setAcceptedMetaTypeIds(const QList<int> &ids);

        //! Accepted ids
        void addAcceptedMetaTypeId(int id);

        //! Drop allowed?
        virtual bool isDropAllowed() const { return m_allowDrop; }

        //! File drop allowed?
        virtual bool isJsonFileDropAllowed() const { return m_acceptJsonFile; }

        //! Drop allowed
        virtual void allowDrop(bool allowed) { m_allowDrop = allowed; }

        //! Allow JSON file drop
        virtual void allowFileDrop(bool allow) { m_acceptJsonFile = allow; }

        //! Accept drop?
        bool acceptDrop(const QMimeData *mime) const;

        //! Mime data to CVariant (normally encapsulating a value object)
        BlackMisc::CVariant toCVariant(const QMimeData *mime) const;

    protected:
        //! Ctor
        CDropBase();

    private:
        bool m_allowDrop = true; //!< dropping allowed?
        bool m_acceptJsonFile = false; //!< accept JSON files
        QList<int> m_acceptedMetaTypes; //!< accepted meta types
    };
} // ns

#endif // guard
