#pragma once

#include "envoy/extensions/filters/http/eventflow/v3/eventflow.pb.h"
#include "envoy/extensions/filters/http/eventflow/v3/eventflow.pb.validate.h"

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace EventFlow {

/**
 * Config registration for the eventflow filter. @see NamedHttpFilterConfigFactory.
 */
class EventFlowFilterFactory
    : public Common::FactoryBase<envoy::extensions::filters::http::eventflow::v3::Config> {
public:
  EventFlowFilterFactory() : FactoryBase(HttpFilterNames::get().EventFlow) {}

private:
  Http::FilterFactoryCb
  createFilterFactoryFromProtoTyped(const envoy::extensions::filters::http::eventflow::v3::Config& proto_config,
                                    const std::string& stats_prefix,
                                    Server::Configuration::FactoryContext& context) override;
};

DECLARE_FACTORY(EventFlowFilterFactory);

} // namespace EventFlow
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
