#pragma once

#include "envoy/config/filter/http/compression/v2alpha/compression.pb.h"
#include "envoy/http/filter.h"
#include "envoy/http/header_map.h"
#include "envoy/json/json_object.h"
#include "envoy/runtime/runtime.h"
#include "envoy/stats/scope.h"
#include "envoy/stats/stats_macros.h"

#include "common/buffer/buffer_impl.h"
#include "common/compressor/zlib_compressor_impl.h"
#include "common/http/header_map_impl.h"
#include "common/json/config_schemas.h"
#include "common/json/json_validator.h"
#include "common/protobuf/protobuf.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Compression {

/**
 * All compression filter stats. @see stats_macros.h
 * "total_uncompressed_bytes" only includes bytes
 * from requests that were marked for compression.
 * If the request was not marked for compression,
 * the filter increments "not_compressed", but does
 * not add to "total_uncompressed_bytes". This way,
 * the user can measure the memory performance of the
 * compression.
 */
// clang-format off
#define ALL_COMPRESSION_STATS(COUNTER)    \
  COUNTER(compressed)              \
  COUNTER(not_compressed)          \
  COUNTER(no_accept_header)        \
  COUNTER(header_identity)         \
  COUNTER(header_gzip)             \
  COUNTER(header_wildcard)         \
  COUNTER(header_not_valid)        \
  COUNTER(total_uncompressed_bytes)\
  COUNTER(total_compressed_bytes)  \
  COUNTER(content_length_too_small)\
  COUNTER(not_compressed_etag)     \
// clang-format on

/**
 * Struct definition for gzip stats. @see stats_macros.h
 */
struct CompressionStats {
  ALL_COMPRESSION_STATS(GENERATE_COUNTER_STRUCT)
};

/**
 * Configuration for the gzip filter.
 */
class CompressionFilterConfig {

public:
  CompressionFilterConfig(const envoy::config::filter::http::compression::v2Alpha::Compression& api,
                  const std::string& stats_prefix,
                   Stats::Scope& scope, Runtime::Loader& runtime);

  Runtime::Loader& runtime() { return runtime_; }
  CompressionStats& stats() { return stats_; }

private:
  static CompressionStats generateStats(const std::string& prefix, Stats::Scope& scope) {
    return CompressionStats{ALL_COMPRESSION_STATS(POOL_COUNTER_PREFIX(scope, prefix))};
  }

  GzipStats stats_;
  Runtime::Loader& runtime_;
};

typedef std::shared_ptr<CompressionFilterConfig> CompressionFilterConfigSharedPtr;

/**
 * A filter that compresses data...
 */
class CompressionFilter : public Http::StreamFilter {
public:
  CompressionFilter(CompressionFilterConfigSharedPtr config);

  // Http::StreamFilterBase
  void onDestroy() override{};

  // Http::StreamDecoderFilter
  Http::FilterHeadersStatus decodeHeaders(Http::HeaderMap& headers, bool end_stream) override;
  Http::FilterDataStatus decodeData(Buffer::Instance&, bool) override {
    return Http::FilterDataStatus::Continue;
  }
  Http::FilterTrailersStatus decodeTrailers(Http::HeaderMap&) override {
    return Http::FilterTrailersStatus::Continue;
  }
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override {
    decoder_callbacks_ = &callbacks;
  };

  // Http::StreamEncoderFilter
  Http::FilterHeadersStatus encode100ContinueHeaders(Http::HeaderMap&) override {
    return Http::FilterHeadersStatus::Continue;
  }
  Http::FilterHeadersStatus encodeHeaders(Http::HeaderMap& headers, bool end_stream) override;
  Http::FilterDataStatus encodeData(Buffer::Instance& buffer, bool end_stream) override;
  Http::FilterTrailersStatus encodeTrailers(Http::HeaderMap&) override {
    return Http::FilterTrailersStatus::Continue;
  }
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override {
    encoder_callbacks_ = &callbacks;
  }

private:
  FilterConfigSharedPtr config_;
  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{nullptr};
  Http::StreamEncoderFilterCallbacks* encoder_callbacks_{nullptr};
};

} // namespace Compression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
