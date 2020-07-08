#include "extensions/filters/http/eventflow/config.h"

#include "extensions/filters/http/eventflow/eventflow_filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace EventFlow {

Http::FilterFactoryCb EventFlowFilterFactory::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::eventflow::v3::Config& proto_config,
    const std::string&, Server::Configuration::FactoryContext&) {
  EventFlowFilterConfigSharedPtr config = std::make_shared<EventFlowFilterConfig>(proto_config);
  return [config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<EventFlowFilter>(config));
  };
}

/**
 * Static registration for the eventflow filter. @see NamedHttpFilterConfigFactory.
 */
REGISTER_FACTORY(EventFlowFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory){"envoy.eventflow"};

} // namespace EventFlow
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
