#include "extensions/filters/http/compression/compression_filter.h"

#include "envoy/stats/scope.h"

#include "common/common/macros.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Compression {


CompressionFilterConfig::CompressionFilterConfig(const envoy::config::filter::http::compression::v2alpha::Compression&,
                                   const std::string& stats_prefix, Stats::Scope& scope,
                                   Runtime::Loader& runtime)
    : stats_(generateStats(stats_prefix + "compression.", scope)), runtime_(runtime) {}


CompressionFilter::CompressionFilter(FilterConfigSharedPtr config)
    : config_(config) {}

Http::FilterHeadersStatus CompressionFilter::decodeHeaders(Http::HeaderMap&, bool) {
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterHeadersStatus CompressionFilter::encodeHeaders(Http::HeaderMap&, bool) {
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus CompressionFilter::encodeData(Buffer::Instance&, bool) {
  return Http::FilterDataStatus::Continue;
}

} // namespace Compression
} // namespace HttpFilters
} // namespace Extensions
