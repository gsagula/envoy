load("@envoy_api//bazel:api_build_system.bzl", "api_proto_library_internal")

licenses(["notice"])  # Apache 2

api_proto_library_internal(
    name = "compression",
    srcs = ["compression.proto"],
    deps = [
        "//envoy/api/v2/core:base",
        "//envoy/type/compression:gzip",
        "//envoy/type/matcher:number",
        "//envoy/type/matcher:string",
    ],
)
