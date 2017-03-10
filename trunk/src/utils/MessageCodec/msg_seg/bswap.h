#ifndef MSG_BSWAP_H_

#define MSG_BSWAP_H_

///
/// @brief
///     16位数的大小端转换
///
#define BSWAP_16(x) (uint16_t)( \
	((*(uint16_t*)(&x) & 0xff00) >> 8) | \
	((*(uint16_t*)(&x) & 0x00ff) << 8) \
	)

///
/// @brief
///     32位数的大小端转换
///
#define BSWAP_32(x) (uint32_t)( \
	(((uint32_t)(x) & 0xff000000) >> 24) | \
	(((uint32_t)(x) & 0x00ff0000) >> 8 ) | \
	(((uint32_t)(x) & 0x0000ff00) << 8 ) | \
	(((uint32_t)(x) & 0x000000ff) << 24) \
	)

///
/// @brief
///     64位数的大小端转换
///
#define BSWAP_64(x) (uint64_t)( \
	(((uint64_t)(x) & 0xff00000000000000) >> 56) | \
	(((uint64_t)(x) & 0x00ff000000000000) >> 40) | \
	(((uint64_t)(x) & 0x0000ff0000000000) >> 24) | \
	(((uint64_t)(x) & 0x000000ff00000000) >> 8 ) | \
	(((uint64_t)(x) & 0x00000000ff000000) << 8 ) | \
	(((uint64_t)(x) & 0x0000000000ff0000) << 24) | \
	(((uint64_t)(x) & 0x000000000000ff00) << 40) | \
	(((uint64_t)(x) & 0x00000000000000ff) << 56) \
	)

#endif
