#pragma once

#include "../utils/thread.h"
#include "../network/node.h"

class Application : public Node
{
public:
    Application(size_t idx, PseudoNetwork *network) : Node(idx, network) {}
};