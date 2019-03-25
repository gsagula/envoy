#pragma once

#include "envoy/config/filter/http/decompression/v2alpha/decompression.pb.h"
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
namespace Decompression {

/**
 * All decompression filter stats. @see stats_macros.h
 * "total_uncompressed_bytes" only includes bytes
 * from requests that were marked for decompression.
 * If the request was not marked for decompression,
 * the filter increments "not_compressed", but does
 * not add to "total_uncompressed_bytes". This way,
 * the user can measure the memory performance of the
 * decompression.
 */
// clang-format off
#define ALL_DECOMPRESSION_STATS(COUNTER)    \
  COUNTER(decompressed)                     \
  COUNTER(not_decompressed)                 \
  COUNTER(total_compressed_bytes)           \
  COUNTER(total_decompressed_bytes)         \
  COUNTER(content_length_too_small)         \
// clang-format on

/**
 * Struct definition for gzip stats. @see stats_macros.h
 */
struct DecompressionStats {
  ALL_DECOMPRESSION_STATS(GENERATE_COUNTER_STRUCT)
};

/**
 * Configuration for the gzip filter.
 */
class DecompressionFilterConfig {

public:
  DecompressionFilterConfig(const envoy::config::filter::http::decompression::v2alpha::Decompression& api,
                  const std::string& stats_prefix,
                   Stats::Scope& scope, Runtime::Loader& runtime);

  Runtime::Loader& runtime() { return runtime_; }
  DecompressionStats& stats() { return stats_; }

private:
  static DecompressionStats generateStats(const std::string& prefix, Stats::Scope& scope) {
    return DecompressionStats{ALL_DECOMPRESSION_STATS(POOL_COUNTER_PREFIX(scope, prefix))};
  }

  DecompressionStats stats_;
  Runtime::Loader& runtime_;
};

typedef std::shared_ptr<DecompressionFilterConfig> DecompressionFilterConfigSharedPtr;

/**
 * A filter that compresses data...
 */
class DecompressionFilter : public Http::StreamFilter {
public:
  DecompressionFilter(DecompressionFilterConfigSharedPtr config) : config_(config) {}

  // Http::StreamFilterBase
  void onDestroy() override {};

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
  Http::FilterMetadataStatus encodeMetadata(Http::MetadataMap&) override {
    return Http::FilterMetadataStatus::Continue;
  }
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override {
    encoder_callbacks_ = &callbacks;
  }

private:
  DecompressionFilterConfigSharedPtr config_;
  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{nullptr};
  Http::StreamEncoderFilterCallbacks* encoder_callbacks_{nullptr};
};

} // namespace Decompression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
