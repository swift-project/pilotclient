/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CRYPTO_CRYPTODTOHEADERDTO_H
#define BLACKCORE_AFV_CRYPTO_CRYPTODTOHEADERDTO_H

#include "blackcore/afv/crypto/cryptodtomode.h"
#include "msgpack.hpp"
#include <QString>
#include <cstdint>

namespace BlackCore::Afv::Crypto
{
    //! DTO header
    struct CryptoDtoHeaderDto
    {
        std::string   ChannelTag; //!< channel
        uint64_t      Sequence;   //!< sequence
        CryptoDtoMode Mode;       //!< mode
        MSGPACK_DEFINE(ChannelTag, Sequence, Mode)
    };
} // ns

#endif // gaurd
