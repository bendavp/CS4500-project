#pragma once

#include "key.h"
#include "value.h"
#include "../utils/map.h"

class kvstore : public OOMap<Key, Value>
{
public:
    kvstore() : OOMap<Key, Value>() {}

    /**
     * @brief Get the And Wait -- waits for the key? TO BE IMPLEMENTED WHEN WE HAVE NETWORKING
     * 
     * @param key 
     * @return Value* 
     */
    Value *WaitAndGet(Key *key)
    {
        assert(key != nullptr);
        return nullptr;
    }
};

// makes a DataFrame with the given float array (as a float column); puts DataFrame as Value into given KVstore with the given Key
DataFrame *DataFrame::fromArray(Key *key, kvstore *kv, size_t sz, float *vals)
{
    Schema *scm = new Schema();
    DataFrame *df = new DataFrame(*scm);
    Column *fc = new FloatColumn();
    for (int i = 0; i < sz; i++)
    {
        fc->push_back(vals[i]);
    }
    df->add_column(fc, nullptr);
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
    df->add_column(fc, nullptr);
    kv->add(key, new Value(df));
    return df;
}