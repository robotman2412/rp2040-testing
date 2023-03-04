
#pragma once

/*
template<typename VarT>
VarT unaligned_read(const VarT &in) {
	VarT out;
	memcpy((void *) &out, (const void *) &in, sizeof(VarT));
	return out;
}

template<typename VarT>
void unaligned_write(VarT &out, VarT in) {
	memcpy((void *) &out, (const void *) &in, sizeof(VarT));
}
*/

static inline uint8_t  unaligned_read(const uint8_t  &in) { return in; }
static inline int8_t   unaligned_read(const int8_t   &in) { return in; }

static inline uint16_t unaligned_read(const uint16_t &in) {
	const uint8_t *ptr = (const uint8_t *) &in;
	return ptr[0] | ((uint16_t) ptr[1] << 8);
}
static inline int16_t  unaligned_read(const int16_t  &in) {
	const uint8_t *ptr = (const uint8_t *) &in;
	return ptr[0] | ((uint16_t) ptr[1] << 8);
}

static inline uint32_t unaligned_read(const uint32_t &in) {
	const uint8_t *ptr = (const uint8_t *) &in;
	return ptr[0]
		| ((uint32_t) ptr[1] <<  8)
		| ((uint32_t) ptr[2] << 16)
		| ((uint32_t) ptr[3] << 24);
}
static inline int32_t  unaligned_read(const int32_t  &in) {
	const uint8_t *ptr = (const uint8_t *) &in;
	return ptr[0]
		| ((uint32_t) ptr[1] <<  8)
		| ((uint32_t) ptr[2] << 16)
		| ((uint32_t) ptr[3] << 24);
}

static inline uint64_t unaligned_read(const uint64_t &in) {
	const uint8_t *ptr = (const uint8_t *) &in;
	return ptr[0]
		| ((uint64_t) ptr[1] <<  8)
		| ((uint64_t) ptr[2] << 16)
		| ((uint64_t) ptr[3] << 24)
		| ((uint64_t) ptr[4] << 32)
		| ((uint64_t) ptr[5] << 40)
		| ((uint64_t) ptr[6] << 48)
		| ((uint64_t) ptr[7] << 56);
}
static inline int64_t  unaligned_read(const int64_t  &in) {
	const uint8_t *ptr = (const uint8_t *) &in;
	return ptr[0]
		| ((uint64_t) ptr[1] <<  8)
		| ((uint64_t) ptr[2] << 16)
		| ((uint64_t) ptr[3] << 24)
		| ((uint64_t) ptr[4] << 32)
		| ((uint64_t) ptr[5] << 40)
		| ((uint64_t) ptr[6] << 48)
		| ((uint64_t) ptr[7] << 56);
}



static inline void unaligned_write(uint8_t  &out, uint8_t  in) { out = in; }
static inline void unaligned_write(int8_t   &out, int8_t   in) { out = in; }

static inline void unaligned_write(uint16_t &out, uint16_t in) {
	uint8_t *ptr = (uint8_t *) &out;
	ptr[0] = in;
	ptr[1] = in >> 8;
}
static inline void unaligned_write(int16_t  &out, int16_t  in) {
	uint8_t *ptr = (uint8_t *) &out;
	ptr[0] = in;
	ptr[1] = in >> 8;
}

static inline void unaligned_write(uint32_t &out, uint32_t in) {
	uint8_t *ptr = (uint8_t *) &out;
	ptr[0] = in;
	ptr[1] = in >>  8;
	ptr[2] = in >> 16;
	ptr[3] = in >> 24;
}
static inline void unaligned_write(int32_t  &out, int32_t  in) {
	uint8_t *ptr = (uint8_t *) &out;
	ptr[0] = in;
	ptr[1] = in >>  8;
	ptr[2] = in >> 16;
	ptr[3] = in >> 24;
}

static inline void unaligned_write(uint64_t &out, uint64_t in) {
	uint8_t *ptr = (uint8_t *) &out;
	ptr[0] = in;
	ptr[1] = in >>  8;
	ptr[2] = in >> 16;
	ptr[3] = in >> 24;
	ptr[4] = in >> 32;
	ptr[5] = in >> 40;
	ptr[6] = in >> 48;
	ptr[7] = in >> 56;
}
static inline void unaligned_write(int64_t  &out, int64_t  in) {
	uint8_t *ptr = (uint8_t *) &out;
	ptr[0] = in;
	ptr[1] = in >>  8;
	ptr[2] = in >> 16;
	ptr[3] = in >> 24;
	ptr[4] = in >> 32;
	ptr[5] = in >> 40;
	ptr[6] = in >> 48;
	ptr[7] = in >> 56;
}
