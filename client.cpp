
#include "client.hpp"
#include "state.hpp"
#include "artifact.hpp"
#include <botan/base64.h>
#include <rtc> // RTC::now()

namespace mender {

  Client::Client(Auth_manager&& man, Device&& dev, net::TCP& tcp, const std::string& server, const uint16_t port)
    : am_{std::forward<Auth_manager>(man)},
      device_{std::forward<Device>(dev)},
      server_(server),
      cached_{0, port},
      httpclient_{std::make_unique<http::Client>(tcp)},
      state_(&state::Init::instance())
  {
    printf("<Client> Client created\n");
  }

  Client::Client(Auth_manager&& man, Device&& dev, net::TCP& tcp, net::tcp::Socket socket)
    : am_{std::forward<Auth_manager>(man)},
      device_{std::forward<Device>(dev)},
      server_{socket.address().to_string()},
      cached_(std::move(socket)),
      httpclient_{std::make_unique<http::Client>(tcp)},
      state_(&state::Init::instance())
  {
    printf("<Client> Client created\n");
  }

  void Client::boot()
  {
    if(liu::LiveUpdate::is_resumable(device_.update_loc()))
    {
      printf("<Client> Found resumable state, try restoring...\n");
      auto success = liu::LiveUpdate::resume(device_.update_loc(), {this, &Client::load_state});
      if(!success)
        printf("<Client> Failed.\n");
    }

    run_state();
  }

  void Client::run_state()
  {
    switch(state_->handle(*this, context_))
    {
      using namespace state;
      case State::Result::GO_NEXT:
        run_state();
        return;

      case State::Result::DELAYED_NEXT:
        run_state_delayed();
        return;

      case State::Result::AWAIT_EVENT:
        // todo: setup timeout
        return;
    }
  }

  void Client::make_auth_request()
  {
    auto auth = am_.make_auth_request();

    using namespace std::string_literals;
    using namespace http;

    std::string data{auth.data.begin(), auth.data.end()};

    //printf("Signature:\n%s\n", Botan::base64_encode(auth.signature).c_str());

    printf("<Client> Making Auth request\n");
    // Make post
    httpclient_->post(cached_,
      API_PREFIX + "/authentication/auth_requests",
      create_auth_headers(auth.signature),
      {data.begin(), data.end()},
      {this, &Client::response_handler});
  }

  http::Header_set Client::create_auth_headers(const byte_seq& signature) const
  {
    return {
      { http::header::Content_Type, "application/json" },
      { http::header::Accept, "application/json" },
      { "X-MEN-Signature", Botan::base64_encode(signature) }
    };
  }

  void Client::response_handler(http::Error err, http::Response_ptr res)
  {
    if(err) printf("<Client> Error: %s\n", err.to_string().c_str());
    if(!res)
    {
      printf("<Client> No reply.\n");
      //assert(false && "Exiting...");
    }
    else
    {
      if(is_valid_response(*res))
      {
        printf("<Client> Valid Response (payload %u bytes)\n", res->body().size());
      }
      else
      {
        printf("<Client> Invalid response:\n%s\n", res->to_string().c_str());
        //assert(false && "Exiting...");
      }
    }

    context_.response = std::move(res);
    run_state();
  }

  bool Client::is_valid_response(const http::Response& res) const
  {
    return is_json(res) or is_artifact(res);
  }

  bool Client::is_json(const http::Response& res) const
  {
    return res.header().value(http::header::Content_Type).find("application/json") != std::string::npos;
  }

  bool Client::is_artifact(const http::Response& res) const
  {
    return res.header().value(http::header::Content_Type).find("application/vnd.mender-artifact") != std::string::npos;
  }

  void Client::check_for_update()
  {
    printf("<Client> Checking for update\n");

    using namespace http;

    const auto& token = am_.auth_token();
    // Setup headers
    const Header_set headers{
      { header::Content_Type, "application/json" },
      { header::Accept, "application/json" },
      { header::Authorization, "Bearer " + std::string{token.begin(), token.end()}}
    };

    std::string path{API_PREFIX + "/deployments/device/deployments/next"};

    auto artifact_name = device_.inventory().value("artifact_name");
    if(! artifact_name.empty())
      path.append("?artifact_name=").append(std::move(artifact_name)).append("&");

    auto device_type = device_.inventory().value("device_type");
    if(! device_type.empty())
      path.append("?device_type=").append(std::move(device_type));

    httpclient_->get(cached_,
      std::move(path),
      headers, {this, &Client::response_handler});
  }

  void Client::fetch_update(http::Response_ptr res)
  {
    if(res == nullptr)
      res = std::move(context_.response);

    auto uri = parse_update_uri(*res);
    printf("<Client> Fetching update from: %s\n", uri.to_string().to_string().c_str());

    using namespace http;

    const auto& token = am_.auth_token();
    // Setup headers
    const Header_set headers{
      { header::Accept, "application/json;application/vnd.mender-artifact" },
      { header::Authorization, "Bearer " + std::string{token.begin(), token.end()}}
    };

    httpclient_->request(GET, {cached_.address(), uri.port()},
      uri.path().to_string() + "?" + uri.query().to_string(), // note: Add query support in http(?)
      headers, {this, &Client::response_handler});
  }

  http::URI Client::parse_update_uri(http::Response& res)
  {
    using namespace nlohmann;
    auto body = json::parse(res.body().to_string());

    std::string uri = body["artifact"]["source"]["uri"];

    return http::URI{uri};
  }

  void Client::update_inventory_attributes()
  {
    printf("<Client> Uploading attributes\n");

    using namespace http;

    const auto& token = am_.auth_token();
    // Setup headers
    const Header_set headers{
      { header::Content_Type, "application/json" },
      { header::Accept, "application/json" },
      { header::Authorization, "Bearer " + std::string{token.begin(), token.end()}}
    };

    auto data = device_.inventory().json_str();

    httpclient_->request(PATCH, cached_,
      "/api/devices/0.1/inventory/device/attributes",
      headers, data, {this, &Client::response_handler});

    context_.last_inventory_update = RTC::now();
  }

  void Client::install_update(http::Response_ptr res)
  {
    printf("<Client> Installing update ...\n");

    if(res == nullptr)
      res = std::move(context_.response);
    assert(res);

    auto data = res->body().to_string();

    // Process data:
    Artifact artifact{{data.begin(), data.end()}};

    // do stuff with artifact
    // checksum/verify
    // artifact.update() <- this is what liveupdate wants

    //artifact.verify();
    auto& e = artifact.get_update(0);  // returns element with index

    device_.inventory("artifact_name") = "updated";

    httpclient_.release();

    liu::LiveUpdate::begin(device_.update_loc(), {(const char*) e.content(), e.size()}, {this, &Client::store_state});
  }

  void Client::store_state(liu::Storage store, liu::buffer_len len)
  {
    decltype(store)::uid id = 0;

    // SEQNO
    store.add_int(id++, am_.seqno());

    // ARTIFACT_NAME
    store.add_string(id++, device_.inventory("artifact_name"));

    printf("<Client> State stored.\n");
  }

  void Client::load_state(liu::Restore store)
  {
    // SEQNO
    am_.set_seqno(store.as_int()); store.go_next();

    // ARTIFACT_NAME
    device_.inventory("artifact_name") = store.as_string(); store.go_next();

    printf("<Client> State restored.\n");
  }

};
