licenses(["notice"])  # Apache 2

# HTTP L7 filter that performs data decompression
# Public docs: docs/root/configuration/http_filters/decompression_filter.rst

load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "decompression_filter_lib",
    srcs = ["decompression_filter.cc"],
    hdrs = ["decompression_filter.h"],
    deps = [
        "//include/envoy/http:filter_interface",
        "//include/envoy/json:json_object_interface",
        "//include/envoy/runtime:runtime_interface",
        "//source/common/buffer:buffer_lib",
        "//source/common/compressor:compressor_lib",
        "//source/common/http:header_map_lib",
        "//source/common/http:headers_lib",
        "//source/common/json:config_schemas_lib",
        "//source/common/json:json_validator_lib",
        "//source/common/protobuf",
        "@envoy_api//envoy/config/filter/http/decompression/v2alpha:decompression_cc",
    ],
)

envoy_cc_library(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        "//include/envoy/registry",
        "//source/extensions/filters/http:well_known_names",
        "//source/extensions/filters/http/common:factory_base_lib",
        "//source/extensions/filters/http/decompression:decompression_filter_lib",
    ],
)

envoy_cc_library(
    name = "decompression_policy_lib",
    srcs = ["decompression_policy.cc"],
    hdrs = ["decompression_policy.h"],
    deps = [
        "@envoy_api//envoy/config/filter/http/decompression/v2alpha:compression_cc",
    ],
)