#pragma once

#include "../utils/map.h"
#include "../kv-store/key.h"

class KDStore : public OOMap<Key, Key>
{
};