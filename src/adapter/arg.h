#pragma once

#include "../utils/object.h"
#include <stdio.h>

class Arg : public Object
{
public:
    size_t num_nodes;
    const char *file;

    Arg(size_t n, const char *file_)
    {
        num_nodes = n;
        file = file_;
    }
};