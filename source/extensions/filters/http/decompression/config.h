#pragma once

#include "envoy/config/filter/http/decompression/v2alpha/decompression.pb.h"
#include "envoy/config/filter/http/decompression/v2alpha/decompression.pb.validate.h"

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Decompression {

/**
 * Config registration for the decompression filter. @see NamedHttpFilterConfigFactory.
 */
class DecompressionFilterFactory
    : public Common::FactoryBase<envoy::config::filter::http::decompression::v2alpha::Decompression> {
public:
  DecompressionFilterFactory() : FactoryBase(HttpFilterNames::get().Decompression) {}

private:
  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::config::filter::http::decompression::v2alpha::Decompression& config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace Decompression
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
