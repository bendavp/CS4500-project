#pragma once

#include "../utils/map.h"
#include "../kv-store/key.h"

class kdstore : public OOMap<Key, Key>
{
};