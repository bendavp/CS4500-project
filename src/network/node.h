#pragma once

#include "../utils/thread.h"
#include "../kv-store/kv-store.h"
#include "../dataframe/dataframe.h"

class Node : public Thread
{
public:
    kvstore *kvstore;
    size_t index_;

    Node(size_t index)
    {
        index_ = index;
    }

    void add(Key *key, DataFrame *df)
    {
        if (key->home != index_)
        {
            // send the key to the right node
        }
        // this is the correct node for the key
        else
        {
            assert(!kvstore->has(key));
            Value *val = new Value(df);
            kvstore->put(key, val);
        }
    }

    DataFrame *get(Key *key)
    {
        if (key->home != index_)
        {
            // get the key from the right node
        }
        // this is the correct node for the key
        else
        {
            return kvstore->get(key)->decode();
        }
    }

    DataFrame *waitAndGet(Key *key)
    {
        // wait for this key to appear in this node
        while (!kvstore->has(key))
        {
            sleep(5000);
        }
    }
};