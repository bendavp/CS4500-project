#pragma once

#include "../utils/object.h"

class Row;

class Writer : public Object
{
public:
    virtual void visit(Row &r) = 0;

    virtual bool done() = 0;
};

class Reader : public Object
{
public:
    virtual bool visit(Row &r) = 0;
};