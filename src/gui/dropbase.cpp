// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/dropbase.h"
#include "gui/guiutility.h"

#include <QMetaType>
#include <QtGlobal>
#include <QUrl>

using namespace swift::misc;

namespace swift::gui
{
    CDropBase::CDropBase()
    {}

    CDropBase::~CDropBase()
    {}

    void CDropBase::setAcceptedMetaTypeIds(const QList<int> &ids)
    {
        m_acceptedMetaTypes = ids;
    }

    void CDropBase::addAcceptedMetaTypeId(int id)
    {
        m_acceptedMetaTypes.append(id);
    }

    bool CDropBase::acceptDrop(const QMimeData *mime) const
    {
        if (!mime) { return false; }
        if (!m_allowDrop) { return false; }
        if (m_acceptedMetaTypes.isEmpty()) { return false; }

        if (m_acceptJsonFile && CGuiUtility::isMimeRepresentingReadableJsonFile(mime))
        {
            // further checks could go here
            return true;
        }

        if (!CGuiUtility::hasSwiftVariantMimeType(mime)) { return false; }
        const int metaTypeId = CGuiUtility::metaTypeIdFromSwiftDragAndDropData(mime);
        if (metaTypeId == QMetaType::UnknownType) { return false; }
        const bool accept = m_acceptedMetaTypes.contains(metaTypeId);
        return accept;
    }

    CVariant CDropBase::toCVariant(const QMimeData *mime) const
    {
        return CGuiUtility::fromSwiftDragAndDropData(mime);
    }
} // ns
