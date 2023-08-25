// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
        std::string ChannelTag; //!< channel
        uint64_t Sequence; //!< sequence
        CryptoDtoMode Mode; //!< mode
        MSGPACK_DEFINE(ChannelTag, Sequence, Mode)
    };
} // ns

#endif // gaurd
