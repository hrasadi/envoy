licenses(["notice"])  # Apache 2

# L7 HTTP filter which implements CORS processing (https://en.wikipedia.org/wiki/Cross-origin_resource_sharing)
# Public docs: docs/root/configuration/http_filters/cors_filter.rst

load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_extension",
    "envoy_cc_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "eventflow_filter_lib",
    srcs = ["eventflow_filter.cc"],
    hdrs = ["eventflow_filter.h"],
    deps = [
        "//include/envoy/http:codes_interface",
        "//include/envoy/http:filter_interface",
        "//source/common/buffer:buffer_lib",
        "//source/common/common:assert_lib",
        "//source/common/common:enum_to_int",
        "//source/common/http:header_map_lib",
        "//source/common/http:headers_lib",
	"//source/common/http:utility_lib",
        "@envoy_api//envoy/extensions/filters/http/eventflow/v3:pkg_cc_proto",
        "@com_google_googleapis//google/pubsub/v1:pubsub_cc_grpc"
    ],
    external_deps = [
        "grpc",
    ],
)

envoy_cc_extension(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    security_posture = "robust_to_untrusted_downstream",
    deps = [
        "//include/envoy/registry",
        "//include/envoy/server:filter_config_interface",
        "//source/extensions/filters/http:well_known_names",
        "//source/extensions/filters/http/common:factory_base_lib",
        "//source/extensions/filters/http/eventflow:eventflow_filter_lib",
        "@envoy_api//envoy/extensions/filters/http/eventflow/v3:pkg_cc_proto",
    ],
)
