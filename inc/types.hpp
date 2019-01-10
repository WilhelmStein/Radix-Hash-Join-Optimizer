
#pragma once

#include <cstdint>

// Tuple types:
using tuple_key_t     = std::size_t;
using tuple_payload_t = int64_t;
#define TUPLE_PAYLOAD_MAX INT64_MAX;
#define TUPLE_PAYLOAD_MIN INT64_MIN;

// Map types:
using bucket_key_t    = int64_t;

using chain_key_t     = int64_t;

// PsumTable types:
using radix_t         = uint8_t;
