// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_GUI_DROPBASE_H
#define SWIFT_GUI_DROPBASE_H

#include <QFileInfo>
#include <QList>
#include <QMimeData>

#include "gui/swiftguiexport.h"
#include "misc/variant.h"

namespace swift::gui
{
    /*!
     * Utilities for dropping swift value objects
     */
    class SWIFT_GUI_EXPORT CDropBase
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
        swift::misc::CVariant toCVariant(const QMimeData *mime) const;

    protected:
        //! Ctor
        CDropBase();

    private:
        bool m_allowDrop = true; //!< dropping allowed?
        bool m_acceptJsonFile = false; //!< accept JSON files
        QList<int> m_acceptedMetaTypes; //!< accepted meta types
    };
} // namespace swift::gui

#endif // SWIFT_GUI_DROPBASE_H
