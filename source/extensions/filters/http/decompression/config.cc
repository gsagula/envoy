#include "extensions/filters/http/decompression/config.h"

#include "envoy/config/filter/http/decompression/v2alpha/decompression.pb.validate.h"
#include "envoy/registry/registry.h"

#include "extensions/filters/http/decompression/decompression_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Decompression {

Http::FilterFactoryCb DecompressionFilterFactory::createFilterFactoryFromProtoTyped(
    const envoy::config::filter::http::decompression::v2alpha::Decompression& proto_config,
    const std::string& stats_prefix, Server::Configuration::FactoryContext& context) {
  DecompressionFilterConfigSharedPtr config = std::make_shared<DecompressionFilterConfig>(
      proto_config, stats_prefix, context.scope(), context.runtime());
  return [config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<DecompressionFilter>(config));
  };
}

/**
 * Static registration for the decompression filter. @see NamedHttpFilterConfigFactory.
 */
static Registry::RegisterFactory<DecompressionFilterFactory,
                                 Server::Configuration::NamedHttpFilterConfigFactory>
    register_;

} // namespace Decompression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
