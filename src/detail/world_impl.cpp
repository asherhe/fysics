#include "world_impl.hpp"

namespace fysics {

bool World::Impl::is_id_valid(WorldId world_id) const {
  // an id of 0 means invalid
  return id != 0 && world_id == id;
}

} // namespace fysics
