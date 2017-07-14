#ifndef PUFFS_FLATE_H
#define PUFFS_FLATE_H

// Code generated by puffs-gen-c. DO NOT EDIT.

#ifndef PUFFS_BASE_HEADER_H
#define PUFFS_BASE_HEADER_H

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Puffs requires a word size of at least 32 bits because it assumes that
// converting a u32 to usize will never overflow. For example, the size of a
// decoded image is often represented, explicitly or implicitly in an image
// file, as a u32, and it is convenient to compare that to a buffer size.
//
// Similarly, the word size is at most 64 bits because it assumes that
// converting a usize to u64 will never overflow.
#if __WORDSIZE < 32
#error "Puffs requires a word size of at least 32 bits"
#elif __WORDSIZE > 64
#error "Puffs requires a word size of at most 64 bits"
#endif

// PUFFS_VERSION is the major.minor version number as a uint32. The major
// number is the high 16 bits. The minor number is the low 16 bits.
//
// The intention is to bump the version number at least on every API / ABI
// backwards incompatible change.
//
// For now, the API and ABI are simply unstable and can change at any time.
//
// TODO: don't hard code this in base-header.h.
#define PUFFS_VERSION (0x00001)

// puffs_base_buf1 is a 1-dimensional buffer (a pointer and length) plus
// additional indexes into that buffer.
//
// A value with all fields NULL or zero is a valid, empty buffer.
typedef struct {
  uint8_t* ptr;  // Pointer.
  size_t len;    // Length.
  size_t wi;     // Write index. Invariant: wi <= len.
  size_t ri;     // Read  index. Invariant: ri <= wi.
  bool closed;   // No further writes are expected.
} puffs_base_buf1;

// puffs_base_limit1 provides a limited view of a 1-dimensional byte stream:
// its first N bytes. That N can be greater than a buffer's current read or
// write capacity. N decreases naturally over time as bytes are read from or
// written to the stream.
//
// A value with all fields NULL or zero is a valid, unlimited view.
typedef struct puffs_base_limit1 {
  uint64_t* ptr_to_len;            // Pointer to N.
  struct puffs_base_limit1* next;  // Linked list of limits.
} puffs_base_limit1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_reader1;

typedef struct {
  puffs_base_buf1* buf;
  puffs_base_limit1 limit;
} puffs_base_writer1;

#endif  // PUFFS_BASE_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------- Status Codes

// Status codes are int32_t values:
//  - the sign bit indicates a non-recoverable status code: an error
//  - bits 10-30 hold the packageid: a namespace
//  - bits 8-9 are reserved
//  - bits 0-7 are a package-namespaced numeric code
//
// Do not manipulate these bits directly. Use the API functions such as
// puffs_flate_status_is_error instead.
typedef int32_t puffs_flate_status;

#define puffs_flate_packageid 967230  // 0x000ec23e

#define PUFFS_FLATE_STATUS_OK 0                               // 0x00000000
#define PUFFS_FLATE_ERROR_BAD_PUFFS_VERSION -2147483647       // 0x80000001
#define PUFFS_FLATE_ERROR_BAD_RECEIVER -2147483646            // 0x80000002
#define PUFFS_FLATE_ERROR_BAD_ARGUMENT -2147483645            // 0x80000003
#define PUFFS_FLATE_ERROR_CONSTRUCTOR_NOT_CALLED -2147483644  // 0x80000004
#define PUFFS_FLATE_ERROR_UNEXPECTED_EOF -2147483643          // 0x80000005
#define PUFFS_FLATE_SUSPENSION_SHORT_READ 6                   // 0x00000006
#define PUFFS_FLATE_SUSPENSION_SHORT_WRITE 7                  // 0x00000007
#define PUFFS_FLATE_ERROR_CLOSED_FOR_WRITES -2147483640       // 0x80000008

#define PUFFS_FLATE_ERROR_BAD_DISTANCE_CODE_COUNT -1157040128  // 0xbb08f800
#define PUFFS_FLATE_ERROR_BAD_FLATE_BLOCK -1157040127          // 0xbb08f801
#define PUFFS_FLATE_ERROR_BAD_LITERALLENGTH_CODE_COUNT \
  -1157040126  // 0xbb08f802
#define PUFFS_FLATE_ERROR_INCONSISTENT_STORED_BLOCK_LENGTH \
  -1157040125  // 0xbb08f803
#define PUFFS_FLATE_ERROR_INTERNAL_ERROR_INCONSISTENT_N_BITS \
  -1157040124                                                      // 0xbb08f804
#define PUFFS_FLATE_ERROR_TODO_FIXED_HUFFMAN_BLOCKS -1157040123    // 0xbb08f805
#define PUFFS_FLATE_ERROR_TODO_DYNAMIC_HUFFMAN_BLOCKS -1157040122  // 0xbb08f806

bool puffs_flate_status_is_error(puffs_flate_status s);

const char* puffs_flate_status_string(puffs_flate_status s);

// ---------------- Structs

typedef struct {
  // Do not access the private_impl's fields directly. There is no API/ABI
  // compatibility or safety guarantee if you do so. Instead, use the
  // puffs_flate_decoder_etc functions.
  //
  // In C++, these fields would be "private", but C does not support that.
  //
  // It is a struct, not a struct*, so that it can be stack allocated.
  struct {
    puffs_flate_status status;
    uint32_t magic;
    uint64_t scratch;

    uint32_t f_bits;
    uint32_t f_n_bits;
    uint32_t f_wip0;
    uint32_t f_wip1;
    uint32_t f_wip2;

    struct {
      uint32_t coro_susp_point;
      uint32_t v_final;
      uint32_t v_type;
    } c_decode[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_n;
    } c_decode_uncompressed[1];
    struct {
      uint32_t coro_susp_point;
      uint32_t v_bits;
      uint32_t v_n_bits;
      uint32_t v_hlit;
      uint32_t v_hdist;
      uint32_t v_hclen;
    } c_decode_dynamic[1];
  } private_impl;
} puffs_flate_decoder;

// ---------------- Public Constructor and Destructor Prototypes

// puffs_flate_decoder_constructor is a constructor function.
//
// It should be called before any other puffs_flate_decoder_* function.
//
// Pass PUFFS_VERSION and 0 for puffs_version and for_internal_use_only.
void puffs_flate_decoder_constructor(puffs_flate_decoder* self,
                                     uint32_t puffs_version,
                                     uint32_t for_internal_use_only);

void puffs_flate_decoder_destructor(puffs_flate_decoder* self);

// ---------------- Public Function Prototypes

puffs_flate_status puffs_flate_decoder_decode(puffs_flate_decoder* self,
                                              puffs_base_writer1 a_dst,
                                              puffs_base_reader1 a_src);

puffs_flate_status puffs_flate_decoder_decode_uncompressed(
    puffs_flate_decoder* self,
    puffs_base_writer1 a_dst,
    puffs_base_reader1 a_src);

puffs_flate_status puffs_flate_decoder_decode_dynamic(puffs_flate_decoder* self,
                                                      puffs_base_writer1 a_dst,
                                                      puffs_base_reader1 a_src);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // PUFFS_FLATE_H
