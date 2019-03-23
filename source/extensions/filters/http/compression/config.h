#pragma once

#include "envoy/config/filter/http/compression/v2alpha/compression.pb.h"
#include "envoy/config/filter/http/compression/v2alpha/compression.pb.validate.h"

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Compression {

/**
 * Config registration for the compression filter. @see NamedHttpFilterConfigFactory.
 */
class CompressionFilterFactory : public Common::FactoryBase<envoy::config::filter::http::compression::v2alpha::Compression> {
public:
  CompressionFilterFactory() : FactoryBase(HttpFilterNames::get().EnvoyCompression) {}

private:
  Http::FilterFactoryCb
  createFilterFactoryFromProtoTyped(const envoy::config::filter::http::compression::v2alpha::Compression& config,
                                    const std::string& stats_prefix,
                                    Server::Configuration::FactoryContext& context) override;
};

} // namespace Compression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
