#pragma once

#include "../utils/thread.h"
#include "../kv-store/kv-store.h"
#include "../dataframe/dataframe.h"
#include "pseudo-network.h"

/**
 * @brief Represents a node in the network 
 * A node runs its own kvstore and has access to the
 * overall network (pseudonetwork)
 * 
 * @author AT and BK
 */
class Node : public Thread
{
public:
    PseudoNetwork *network_;
    kvstore *kvstore;
    size_t index_;

    Node(PseudoNetwork *network)
    {
        network_ = network;
        network_->register_node();
        index_ = network_->current_node_idx;
    }

    void run()
    {
        Message *m;
        while (true)
        {
            m = network_->recv_m();
            if (!m) // if the message is null, then node sleeps/waits to get the next message from queue
            {
                sleep(1500);
            }
            else // the message is not null, we process it
            {
                processMessage(m);
            }
            delete m;
        }
    }

    void processMessage(Message *m)
    {
        if (m->kind_ == MsgKind::Put)
        {
            add(m->key, m->df);
        }
        else if ()
    }

    void add(Key *key, DataFrame *df)
    {
        Value *val = new Value(df);
        if (key->home != index_)
        {
            // send a message to the network for the specified node (by the Key) to store this Key/Val pair in its KV-store
            network_->send_m(new Put(index_, key, val));
        }
        // this is the correct node for the key
        else
        {
            assert(!kvstore->has(key));
            kvstore->put(key, val);
        }
    }

    DataFrame *get(Key *key)
    {
        if (key->home != index_)
        {
            // send message to network for the specified node to get the Value from this key
            network_->send_m(new Get(index_, key));
        }
        // this is the correct node for the key
        else
        {
            return kvstore->get(key)->decode();
        }
    }

    DataFrame *waitAndGet(Key *key)
    {
        if (key->home != index_)
        {
            // send message to network for the specified node to get the Value from this key
            network_->send_m(new WaitAndGet(index_, key));
        }
        // this is the correct node for the key
        else
        {
            // wait for this key to appear in this node
            while (!kvstore->has(key))
            {
                sleep(5000);
            }
            return kvstore->get(key)->decode();
        }
    }
};