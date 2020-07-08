#pragma once

#include "envoy/extensions/filters/http/eventflow/v3/eventflow.pb.h"

#include "envoy/http/filter.h"
#include "envoy/stats/scope.h"
#include "envoy/stats/stats_macros.h"

#include "common/buffer/buffer_impl.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace EventFlow {


/**
 * Configuration for the EventFlow filter.
 */
class EventFlowFilterConfig {
public:
  EventFlowFilterConfig(const envoy::extensions::filters::http::eventflow::v3::Config& proto_config);
};

using EventFlowFilterConfigSharedPtr = std::shared_ptr<EventFlowFilterConfig>;

class EventFlowFilter : public Http::StreamFilter, protected Logger::Loggable<Logger::Id::filter> {
public:
  EventFlowFilter(EventFlowFilterConfigSharedPtr config);

  // Http::StreamFilterBase
  void onDestroy() override {}

  // Http::StreamDecoderFilter
  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers,
                                          bool end_stream) override;
  Http::FilterDataStatus decodeData(Buffer::Instance&, bool) override {
    return Http::FilterDataStatus::Continue;
  };
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap&) override {
    return Http::FilterTrailersStatus::Continue;
  };
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override;

  // Http::StreamEncoderFilter
  Http::FilterHeadersStatus encode100ContinueHeaders(Http::ResponseHeaderMap&) override {
    return Http::FilterHeadersStatus::Continue;
  }
  Http::FilterHeadersStatus encodeHeaders(Http::ResponseHeaderMap&,
                                          bool) override;
  Http::FilterDataStatus encodeData(Buffer::Instance&, bool) override {
    return Http::FilterDataStatus::Continue;
  };
  Http::FilterTrailersStatus encodeTrailers(Http::ResponseTrailerMap&) override {
    return Http::FilterTrailersStatus::Continue;
  };
  Http::FilterMetadataStatus encodeMetadata(Http::MetadataMap&) override {
    return Http::FilterMetadataStatus::Continue;
  }
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override; 
private:

  void neverMind();

  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{};
  Http::StreamEncoderFilterCallbacks* encoder_callbacks_{};


  EventFlowFilterConfigSharedPtr config_;

  std::unique_ptr<Envoy::Buffer::OwnedImpl> event_data_;

  bool has_ack_id_;
  std::string ack_id_;
};

} // namespace Cors
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
