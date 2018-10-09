#pragma once

#include "envoy/buffer/buffer.h"

namespace Envoy {
namespace Decompressor {

/**
 * Allows decompressing data.
 */
class Decompressor {
public:
  virtual ~Decompressor() {}

  /**
   * Expands the compressed data in a buffer.
   * @param input_buffer supplies the buffer with compressed data.
   */
  virtual void decompress(Buffer::Instance& input_buffer) PURE;
};

} // namespace Decompressor
} // namespace Envoy
