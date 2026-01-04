#pragma once
#include <cstdint>

enum class MsgType : uint8_t {
    HEARTBEAT = 0,
    TRADE = 1,
    QUOTE = 2,
    BOOK_SNAP = 3
};

#pragma pack(push, 1)

struct MsgHeader {
    MsgType type;
    uint16_t size;
};

struct TradeMsg {
    uint64_t timestamp;
    double   price;
    double   quantity;
    char     ticker[8];
    char     side;
};

struct QuoteMsg {
    uint64_t timestamp;
    char     ticker[8];
    double   bid_price;
    double   bid_size;
    double   ask_price;
    double   ask_size;
};

struct HeartbeatMsg {
    uint64_t timestamp;
    uint32_t sequence_id;
};

#pragma pack(pop)