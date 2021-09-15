/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WEATHER_METARDECODER_H
#define BLACKMISC_WEATHER_METARDECODER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/weather/metar.h"

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

namespace BlackMisc::Weather
{
    class IMetarDecoderPart;

    //! Metar Decoder
    class BLACKMISC_EXPORT CMetarDecoder : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CMetarDecoder();

        //! Default destructor
        virtual ~CMetarDecoder() override;

        //! Decode metar
        CMetar decode(const QString &metarString) const;

    private:
        void allocateDecoders();
        std::vector<std::unique_ptr<IMetarDecoderPart>> m_decoders;
    };

} // namespace

#endif // guard
