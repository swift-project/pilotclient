/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/dropbase.h"
#include "blackgui/guiutility.h"

#include <QMetaType>
#include <QtGlobal>
#include <QUrl>

using namespace BlackMisc;

namespace BlackGui
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
