#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "envoy/config/filter/http/ext_authz/v2alpha/ext_authz.pb.h"
#include "envoy/http/filter.h"
#include "envoy/local_info/local_info.h"
#include "envoy/runtime/runtime.h"
#include "envoy/stats/scope.h"
#include "envoy/upstream/cluster_manager.h"

#include "common/common/assert.h"
#include "common/common/logger.h"
#include "common/http/header_map_impl.h"

#include "extensions/filters/common/ext_authz/ext_authz.h"
#include "extensions/filters/common/ext_authz/ext_authz_grpc_impl.h"
#include "extensions/filters/common/ext_authz/ext_authz_http_impl.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace ExtAuthz {

/**
 * Type of requests the filter should apply to.
 */
enum class FilterRequestType { Internal, External, Both };

/**
 * Global configuration for the HTTP authorization (ext_authz) filter.
 */
class FilterConfig {
public:
  FilterConfig(const envoy::config::filter::http::ext_authz::v2alpha::ExtAuthz& config,
               const LocalInfo::LocalInfo& local_info, const Runtime::Loader& runtime,
               Stats::Scope& scope, Upstream::ClusterManager& cm);

  const LocalInfo::LocalInfo& localInfo() { return local_info_; }
  const Runtime::Loader& runtime() { return runtime_; }
  const std::string& cluster() { return cluster_name_; }

  const Http::LowerCaseStrUnorderedSet& allowedUpstreamHeaders() {
    return allowed_upstream_headers_;
  }

  const Http::LowerCaseStrUnorderedSet& allowedClientHeaders() { return allowed_client_headers_; }

  const Http::LowerCaseStrUnorderedSet& allowedRequestHeaders() { return allowed_request_headers_; }

  const Http::LowerCaseStrUnorderedSet& allowedRequestHeaderPrefixes() {
    return allowed_request_headers_prefix_;
  }

  const Http::LowerCaseStringPairVec& authorizationHeadersToAdd() {
    return authorization_headers_to_add_;
  }

  Stats::Scope& scope() const { return scope_; }
  Upstream::ClusterManager& cm() const { return cm_; }

  bool failureModeAllow() const { return failure_mode_allow_; }

private:
  static Http::LowerCaseStrUnorderedSet
  toResquestHeader(const Protobuf::RepeatedPtrField<Envoy::ProtobufTypes::String>& keys);
  static Http::LowerCaseStrUnorderedSet
  toHeaderPrefix(const Protobuf::RepeatedPtrField<Envoy::ProtobufTypes::String>& keys);
  static Http::LowerCaseStringPairVec toAuthorizationHeaderToAdd(
      const Protobuf::RepeatedPtrField<envoy::api::v2::core::HeaderValue>& headers);
  static Http::LowerCaseStrUnorderedSet
  toClientHeader(const Protobuf::RepeatedPtrField<Envoy::ProtobufTypes::MessagePtr>& message);
  static Http::LowerCaseStrUnorderedSet
  toUpstreamHeader(const Protobuf::RepeatedPtrField<Envoy::ProtobufTypes::MessagePtr>& message);

  const Http::LowerCaseStrUnorderedSet allowed_request_headers_;
  const Http::LowerCaseStrUnorderedSet allowed_request_headers_prefix_;
  const Http::LowerCaseStringPairVec authorization_headers_to_add_;
  const Http::LowerCaseStrUnorderedSet allowed_client_headers_;
  const Http::LowerCaseStrUnorderedSet allowed_upstream_headers_;

  const LocalInfo::LocalInfo& local_info_;
  const Runtime::Loader& runtime_;
  const std::string cluster_name_;

  Stats::Scope& scope_;
  Upstream::ClusterManager& cm_;
  bool failure_mode_allow_{};
};

typedef std::shared_ptr<FilterConfig> FilterConfigSharedPtr;

/**
 * HTTP ext_authz filter. Depending on the route configuration, this filter calls the global
 * ext_authz service before allowing further filter iteration.
 */
class Filter : public Logger::Loggable<Logger::Id::filter>,
               public Http::StreamDecoderFilter,
               public Filters::Common::ExtAuthz::RequestCallbacks {
public:
  Filter(FilterConfigSharedPtr config, Filters::Common::ExtAuthz::ClientPtr&& client)
      : config_(config), client_(std::move(client)) {}

  // Http::StreamFilterBase
  void onDestroy() override;

  // Http::StreamDecoderFilter
  Http::FilterHeadersStatus decodeHeaders(Http::HeaderMap& headers, bool end_stream) override;
  Http::FilterDataStatus decodeData(Buffer::Instance& data, bool end_stream) override;
  Http::FilterTrailersStatus decodeTrailers(Http::HeaderMap& trailers) override;
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override;

  // ExtAuthz::RequestCallbacks
  void onComplete(Filters::Common::ExtAuthz::ResponsePtr&&) override;

private:
  void addResponseHeaders(Http::HeaderMap& header_map, const Http::HeaderVector& headers);
  // State of this filter's communication with the external authorization service.
  // The filter has either not started calling the external service, in the middle of calling
  // it or has completed.
  enum class State { NotStarted, Calling, Complete };
  // FilterReturn is used to capture what the return code should be to the filter chain.
  // if this filter is either in the middle of calling the service or the result is denied then
  // the filter chain should stop. Otherwise the filter chain can continue to the next filter.
  enum class FilterReturn { ContinueDecoding, StopDecoding };
  void initiateCall(const Http::HeaderMap& headers);
  Http::HeaderMapPtr getHeaderMap(const Filters::Common::ExtAuthz::ResponsePtr& response);
  FilterConfigSharedPtr config_;
  Filters::Common::ExtAuthz::ClientPtr client_;
  Http::StreamDecoderFilterCallbacks* callbacks_{};
  Http::HeaderMap* request_headers_;
  State state_{State::NotStarted};
  FilterReturn filter_return_{FilterReturn::ContinueDecoding};
  Upstream::ClusterInfoConstSharedPtr cluster_;
  // Used to identify if the callback to onComplete() is synchronous (on the stack) or asynchronous.
  bool initiating_call_{};
  envoy::service::auth::v2alpha::CheckRequest check_request_{};
};

} // namespace ExtAuthz
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
