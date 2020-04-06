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
    size_t idx_;
    Lock lock_;
    Reply *reply_;

    Node(size_t idx, PseudoNetwork *network)
    {
        network_ = network;
        idx_ = idx;
    }

    void run()
    {
        network_->register_node(idx_);
        while (true)
        {
            // talks to network and receives msg
            Message *m = network_->recv_m();
            if (m->kind_ == MsgKind::Reply)
            {
                reply_ = dynamic_cast<Reply *>(m); // notifies
                lock_.notify_all();
            }
            else if (m->kind_ == MsgKind::Get)
            {
                Get *getMsg = dynamic_cast<Get *>(m);
                Value *v = kvstore->get(getMsg->key_);
                Reply *reMsg = new Reply(idx_, getMsg->sender_, v);
                network_->send_m(reMsg);
                delete getMsg;
            }
            else if (m->kind_ == MsgKind::WaitAndGet)
            {
                WaitAndGet *wagMsg = dynamic_cast<WaitAndGet *>(m);
                Value *v = waitAndGet(wagMsg->key_);
                Reply *reMsg = new Reply(idx_, wagMsg->sender_, v);
                network_->send_m(reMsg);
                delete wagMsg;
            }
            else if (m->kind_ == MsgKind::Put)
            {
                Put *putMsg = dynamic_cast<Put *>(m);
                add(putMsg->key_, putMsg->val_);
                delete putMsg;
            }
            else if (m->kind_ == MsgKind::Kill)
            {
                join();
            }
        }
    }

    void add(Key *key, Value *val)
    {
        if (key->home != idx_)
        {
            // send a message to the network for the specified node (by the Key) to store this Key/Val pair in its KV-store
            network_->send_m(new Put(idx_, key, val));
        }
        // this is the correct node for the key
        else
        {
            assert(!kvstore->has(key));
            kvstore->add(key, val);
        }
    }

    Value *get(Key *key)
    {
        if (key->home != idx_)
        {
            // get the key from the right node
            // send message (give me value for key)
            network_->send_m(new Get(0, key));
            lock_.wait();
            Reply *r_msg = reply_;  // returns a reply message
            Value *v = r_msg->val_; // gets the val from msg
            // DataFrame *df = v->decode_df();
            delete r_msg;
            return v;
        }
        // this is the correct node for the key
        else
        {
            return kvstore->get(key);
        }
    }

    Value *waitAndGet(Key *key)
    {
        if (key->home != idx_)
        {
            // send message to network for the specified node to get the Value from this key
            network_->send_m(new WaitAndGet(idx_, key));
            lock_.wait();
            Reply *r_msg = reply_;  // returns a reply message
            Value *v = r_msg->val_; // gets the val from msg
            delete r_msg;
            return v;
        }
        // this is the correct node for the key
        else
        {
            // wait for this key to appear in this node
            while (!kvstore->has(key))
            {
                sleep(5000);
            }
            return kvstore->get(key);
        }
    }
};