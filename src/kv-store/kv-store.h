#pragma once

#include "key.h"
#include "value.h"
#include "../utils/map.h"

class kvstore : public Map<Key *, Value *>
{
public:
    kvstore() : Map<Key *, Value *>() {}
};

// makes a DataFrame with the given float array (as a float column); puts DataFrame as Value into given KVstore with the given Key
DataFrame *DataFrame::fromArray(Key *key, kvstore *kv, size_t sz, float *vals)
{
    Schema *scm = new Schema();
    DataFrame *df = new DataFrame(*scm);
    Column *fc = new FloatColumn();
    for (size_t i = 0; i < sz; i++)
    {
        fc->push_back(vals[i]);
    }
    df->add_column(fc);
    kv->add(key, new Value(df));
    return df;
}

// makes a DataFrame with the given float val; puts DataFrame as Value into given KVstore with the given Key
DataFrame *DataFrame::fromScalar(Key *key, kvstore *kv, float sum)
{
    Schema *scm = new Schema();
    DataFrame *df = new DataFrame(*scm);
    Column *fc = new FloatColumn();
    fc->push_back(sum);
    df->add_column(fc);
    kv->add(key, new Value(df));
    return df;
}

DataFrame *DataFrame::fromVisitor(Key *key, kvstore *kvstore, const char *schema, Writer &w)
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