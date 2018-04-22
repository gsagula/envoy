#pragma once

#include <string>
#include <vector>

#include "extensions/filters/common/ext_authz/ext_authz.h"

#include "gmock/gmock.h"

namespace Envoy {
namespace Extensions {
namespace Filters {
namespace Common {
namespace ExtAuthz {

class MockClient : public Client {
public:
  MockClient();
  ~MockClient();

  // ExtAuthz::Client
  MOCK_METHOD0(cancel, void());
  MOCK_METHOD3(check, void(RequestCallbacks& callbacks,
                           const envoy::service::auth::v2alpha::CheckRequest& request,
                           Tracing::Span& parent_span));
};

class MockResponse : public Response {
public:
  MockResponse();
  ~MockResponse();

  // ExtAuthz::Response
  MOCK_METHOD0(status, CheckStatus());
  MOCK_METHOD0(headers, const HeaderKeyValuePair&());
  MOCK_METHOD0(body, Buffer::Instance&());
};

} // namespace ExtAuthz
} // namespace Common
} // namespace Filters
} // namespace Extensions
} // namespace Envoy
