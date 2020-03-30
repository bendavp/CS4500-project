#pragma once

#include <stdio.h>
#include "../utils/string.h"
#include "../kv-store/key.h"
#include "../kv-store/value.h"

/**
 * @brief The types of allowed messages:
 * Put: Put a Key, Value pair into the KVstore
 * Get: Get a Value from the KVstore
 * WaitAndGet: Get a Value from the KVstore when Key exists
 * 
 * The rest of the messages are for a network, rather than a pseudo-network,
 * and for that reason we are not implementing them at this time (since we are
 * only implementing a pseudo-network)
 * 
 * @author jv
 */
enum class MsgKind
{
    Ack,
    Nack,
    Put,

    Reply,
    Get,
    WaitAndGet,
    Status,

    Kill,
    Register,
    Directory
};

/**
 * @brief The skeleton of a message
 * 
 * @param kind_ The kind of message this message is (see enum above)
 * @param sender_ the index of the node of the sender of this message
 * @param target_ the index of the node of the receiver of this message
 * 
 * @author jv 
 * 
 */
class Message : public Object
{
public:
    MsgKind kind_; // the message kind

    size_t sender_; // the index of the sender node

    size_t target_; // the index of the receiver node
};

/**
 * @brief Message to get a Value from a specified Node's (based off of the Key) KV-store with the given Key
 * 
 * @author BK and AT
 */
class Get : public Message
{
public:
    Key *key_;

    Get(size_t sender, Key *key)
    {
        kind_ = MsgKind::Get;
        key_ = key;
        target_ = key_->home; // getting the node that the key is on
        sender_ = sender;
    }

    size_t hash_me() override
    {
        return key_->hash() + target_ + sender_;
    }
};

/**
 * @brief 
 * 
 * @author BK and AT
 */
class WaitAndGet : public Message
{
public:
    Key *key_;

    WaitAndGet(size_t sender, Key *key)
    {
        kind_ = MsgKind::WaitAndGet;
        key_ = key;
        target_ = key_->home; // getting the node that the key is on
        sender_ = sender;
    }

    size_t hash_me() override
    {
        return key_->hash() + target_ + sender_;
    }
};

/**
 * @brief Message to put the given Key and Value into the specified Node (based on the Key)
 * 
 * @author AT and BK
 */
class Put : public Message
{
public:
    Key *key_;
    Value *val_;

    Put(size_t sender, Key *key, Value *val)
    {
        kind_ = MsgKind::Put;
        key_ = key;
        val_ = val;
        target_ = key_->home;
        sender_ = sender;
    }

    size_t hash_me() override
    {
        return key_->hash() + val_->hash() + target_ + sender_;
    }
};