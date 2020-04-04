#pragma once

#include "../utils/thread.h"
#include "../network/node.h"

class Application : public Thread
{
public:
    size_t idx_;
    Node *node_;

    Application(size_t idx, Node *node) : Thread()
    {
        idx_ = idx;
        node_ = node;
    }
};