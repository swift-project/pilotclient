/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "reader_settings.h"

namespace BlackCore
{
    CReaderSettings::CReaderSettings() :
        // m_protocolIcaoReader("http"), m_serverIcaoReader("vatrep.vatsim-germany.org"),
        // m_protocolModelReader("http"), m_serverModelReader("vatrep.vatsim-germany.org"),
        m_protocolIcaoReader("http"), m_serverIcaoReader("ubuntu12"),  m_baseUrlIcaoReader("vatrep/public"),
        m_protocolModelReader("http"), m_serverModelReader("ubuntu12"), m_baseUrlModelReader("vatrep/public"),
        m_bookingsUrl("http://vatbook.euroutepro.com/xml2.php"),
        m_vatsimDataFileUrls({ "http://info.vroute.net/vatsim-data.txt" })
    { }

    const CReaderSettings &CReaderSettings::instance()
    {
        static const CReaderSettings rs;
        return rs;
    }
}
