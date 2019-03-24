#include "extensions/filters/http/compression/config.h"

#include "envoy/config/filter/http/compression/v2alpha/compression.pb.validate.h"
#include "envoy/registry/registry.h"

#include "extensions/filters/http/compression/compression_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Compression {

Http::FilterFactoryCb CompressionFilterFactory::createFilterFactoryFromProtoTyped(
    const envoy::config::filter::http::compression::v2alpha::Compression& proto_config,
    const std::string& stats_prefix, Server::Configuration::FactoryContext& context) {
  CompressionFilterConfigSharedPtr config = std::make_shared<CompressionFilterConfig>(
      proto_config, stats_prefix, context.scope(), context.runtime());
  return [config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<CompressionFilter>(config));
  };
}

/**
 * Static registration for the compression filter. @see NamedHttpFilterConfigFactory.
 */
static Registry::RegisterFactory<CompressionFilterFactory,
                                 Server::Configuration::NamedHttpFilterConfigFactory>
    register_;

} // namespace Compression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
