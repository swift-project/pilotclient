#ifndef CRYPTODTOHEADERDTO_H
#define CRYPTODTOHEADERDTO_H

#include "cryptodtomode.h"
#include "msgpack.hpp"
#include <QString>
#include <cstdint>

struct CryptoDtoHeaderDto
{
    std::string ChannelTag;
    uint64_t Sequence;
    CryptoDtoMode Mode;
    MSGPACK_DEFINE(ChannelTag, Sequence, Mode)
};

#endif // CRYPTODTOHEADERDTO_H
