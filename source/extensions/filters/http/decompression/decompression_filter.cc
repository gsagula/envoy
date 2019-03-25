#include "extensions/filters/http/decompression/decompression_filter.h"

#include "envoy/stats/scope.h"

#include "common/common/macros.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Decompression {

DecompressionFilterConfig::DecompressionFilterConfig(
    const envoy::config::filter::http::decompression::v2alpha::Decompression&,
    const std::string& stats_prefix, Stats::Scope& scope, Runtime::Loader& runtime)
    : stats_(generateStats(stats_prefix + "decompression.", scope)), runtime_(runtime) {}

Http::FilterHeadersStatus DecompressionFilter::decodeHeaders(Http::HeaderMap&, bool) {
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterHeadersStatus DecompressionFilter::encodeHeaders(Http::HeaderMap&, bool) {
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus DecompressionFilter::encodeData(Buffer::Instance&, bool) {
  return Http::FilterDataStatus::Continue;
}

} // namespace Decompression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
