#pragma once

#include "../utils/thread.h"
#include "../network/pseudo-network.h"
#include "../network/node.h"

class Application : public Node
{
public:
    size_t idx_;

    Application(size_t idx, PseudoNetwork *network) : Node(network)
    {
        idx_ = idx;
    }
};