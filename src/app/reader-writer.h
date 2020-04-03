#pragma once

#include "../dataframe/dataframe.h"
#include "../kv-store/kv-store.h"

class Writer : public Object
{
public:
    virtual void visit(Row &r);

    virtual bool done();
};

class Reader : public Object
{
public:
    virtual bool visit(Row &r);
};

DataFrame *DataFrame::fromVisitor(Key *key, kvstore *kvstore, char *schema, Writer &w)
{
    Schema scm(schema);
    DataFrame *df = new DataFrame(scm);

    Row r = Row(scm);
    while (!w.done())
    {
        w.visit(r);
        df->add_row(r);
    }

    Value *v = new Value(df);
    kvstore->add(key, v);

    return df;
}