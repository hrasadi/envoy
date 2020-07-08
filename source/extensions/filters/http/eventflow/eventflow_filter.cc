#include "extensions/filters/http/eventflow/eventflow_filter.h"

#include "envoy/http/codes.h"
#include "envoy/stats/scope.h"

#include "common/common/empty_string.h"
#include "common/common/enum_to_int.h"
#include "common/http/header_map_impl.h"
#include "common/http/headers.h"
#include "common/http/utility.h"


#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>

#include "google/pubsub/v1/pubsub.grpc.pb.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace EventFlow {

EventFlowFilterConfig::EventFlowFilterConfig(const envoy::extensions::filters::http::eventflow::v3::Config&) {
}

EventFlowFilter::EventFlowFilter(EventFlowFilterConfigSharedPtr config)
    : config_(std::move(config)) {
    	event_data_ = std::make_unique<Envoy::Buffer::OwnedImpl>();
    }

Http::FilterHeadersStatus EventFlowFilter::decodeHeaders(Http::RequestHeaderMap& header_map, bool) {

    has_ack_id_ = false;
    
    
    using grpc::ClientContext;
    using google::pubsub::v1::Subscriber;
    using google::pubsub::v1::PullRequest;
    using google::pubsub::v1::PullResponse;


    auto creds = grpc::GoogleDefaultCredentials();
    //grpc::InsecureChannelCredentials()) 
    
    
     //auto call_credentials = grpc::GoogleComputeEngineCredentials();
     //auto channel_credentials = grpc::SslCredentials(grpc::SslCredentialsOptions());
     //auto creds = grpc::CompositeChannelCredentials(channel_credentials, call_credentials);

    Subscriber::Stub stub (
        grpc::CreateChannel("pubsub.googleapis.com", creds));

    PullRequest request;
    request.set_subscription(
        "projects/eventflow-qa/subscriptions/envoy-demo-sub");
    request.set_max_messages(1);

    PullResponse response;
    ClientContext ctx;


    auto status = stub.Pull(&ctx, request, &response);

    //grpc_shutdown();

    if (!status.ok()) {
        ENVOY_LOG(warn, "rpc failed. {} {} ", status.error_code(), status.error_message());

  	return Http::FilterHeadersStatus::Continue;
    }

    if (response.received_messages().size() == 0) {
    	neverMind();
	// Do not send the message to upstreami
	return Http::FilterHeadersStatus::StopIteration;
    }

    // Event indicator
    auto a = Http::LowerCaseString(std::string("event"));
    header_map.addCopy(a, "True");


    ack_id_ = response.received_messages()[0].ack_id();
    has_ack_id_ = true;
   
    event_data_->add(std::string(response.received_messages()[0].message().data()));
    //std::string *str = new std::string("asd");
    //event_data_->add(*str);

    ENVOY_LOG(info, "buffer: {}", event_data_->toString());
    ENVOY_LOG(info, "buffer length: {}", event_data_->length());

    header_map.setContentLength(event_data_->length());
    decoder_callbacks_->addDecodedData(*event_data_, false);

    //std::string str = std::string(response.received_messages()[0].message().data());
    //header_map.setContentLength(event_data.length());


    header_map.setMethod("POST");
    

    return Http::FilterHeadersStatus::Continue;
}

void EventFlowFilter::neverMind() {
  decoder_callbacks_->sendLocalReply(Http::Code::OK, "Empty", nullptr, absl::nullopt, "");
}

Http::FilterHeadersStatus EventFlowFilter::encodeHeaders(Http::ResponseHeaderMap& header_map, bool) {

  ENVOY_LOG(info, "Here");

  if (!has_ack_id_ || Http::Utility::getResponseStatus(header_map) != 200) {
    ENVOY_LOG(info, "never mined!");
    grpc_shutdown();
    
    return Http::FilterHeadersStatus::Continue;
  }

  ENVOY_LOG(info, "has_ack_id: {}", has_ack_id_);
  ENVOY_LOG(info, "ack_id: {}", ack_id_);
  ENVOY_LOG(info, "ret value: {}", Http::Utility::getResponseStatus(header_map));  


  using grpc::ClientContext;
  using google::pubsub::v1::Subscriber;
  using google::pubsub::v1::AcknowledgeRequest;


   auto creds = grpc::GoogleDefaultCredentials();
   auto stub = std::make_unique<Subscriber::Stub>(
        grpc::CreateChannel("pubsub.googleapis.com", creds));

    AcknowledgeRequest request;
    request.set_subscription(
        "projects/eventflow-qa/subscriptions/envoy-demo-sub");
    request.add_ack_ids(ack_id_);

    ClientContext ctx;

    auto status = stub->Acknowledge(&ctx, request, nullptr);

    grpc_shutdown();

   return Http::FilterHeadersStatus::Continue;
}

void EventFlowFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

void EventFlowFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
};

} // namespace EventFlow
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
