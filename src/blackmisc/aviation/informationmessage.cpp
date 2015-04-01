/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {

        QString CInformationMessage::convertToQString(bool /** i18n **/) const
        {
            return this->m_message;
        }

        const QString &CInformationMessage::getTypeAsString() const
        {
            switch (this->m_type)
            {
            case ATIS:
                {
                    static const QString atis("ATIS");
                    return atis;
                }
            case METAR:
                {
                    static const QString metar("METAR");
                    return metar;
                }
            case TAF:
                {
                    static const QString taf("TAF");
                    return taf;
                }
            default:
                {
                    static const QString ds("unknown");
                    return ds;
                }
            }
        }

    } // namespace
} // namespace
