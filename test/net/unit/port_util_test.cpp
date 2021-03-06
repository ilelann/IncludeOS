
#include <common.cxx>
#include <net/port_util.hpp>

CASE("Binding and unbinding")
{
  net::Port_util util;

  const uint16_t port = 322;

  EXPECT(util.is_bound(port) == false);

  util.bind(port);

  EXPECT(util.is_bound(port) == true);

  util.unbind(port);

  EXPECT(util.is_bound(port) == false);
}

CASE("Generating ephemeral port throws when all are bound")
{
  net::Port_util util;

  EXPECT(util.has_free_ephemeral() == true);

  // Bind all
  for(auto i = 0; i < util.size(); ++i)
    util.bind(util.get_next_ephemeral());

  EXPECT(util.has_free_ephemeral() == false);

  EXPECT_THROWS_AS(util.get_next_ephemeral(), net::Port_error);
}

CASE("Generating ephemeral handles wrap around")
{
  net::Port_util util;

  auto port = util.get_next_ephemeral();

  // bind first one
  util.bind(port);

  // wrap around
  for(auto i = 0; i < util.size() - 1; ++i)
    util.get_next_ephemeral();

  auto port2 = util.get_next_ephemeral();

  EXPECT(port != port2);
}

CASE("Simulating random connections")
{
  using namespace net;
  srand(time(0));

  for(int rounds = 0; rounds < 10; rounds++)
  {
    Port_util util;
    // random ephemeral
    int p = port_ranges::DYNAMIC_START + rand() % Port_util::size();
    util.bind(p);
    // bind ephemerals
    for (auto i = 0; i < util.size() - 1; ++i) {
        int p = util.get_next_ephemeral();
        util.bind(p);
    }
    EXPECT(util.has_free_ephemeral() == false);
  }
}
