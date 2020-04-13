#pragma once

#include "../utils/queue.h"
#include "message.h"
#include "../utils/thread.h"
#include "../utils/map.h"
#include "../utils/array.h"

class MessageQueue : public Queue<Message>
{
public:
    Lock lock_;

    MessageQueue() : Queue<Message>() {}

    void push(Message *msg)
    {
        lock_.lock();
        add(msg);
        lock_.notify_all();
        lock_.unlock();
    }

    Message *pop()
    {
        lock_.lock();
        while (size_ == 0)
            lock_.wait();
        Message *result = remove();
        lock_.unlock();
        return result;
    }

    MessageQueue *clone()
    {
        MessageQueue *res = new MessageQueue();
        for (int i = 0; i < size_; i++)
        {
            res->add(get_el_at(i)->clone());
        }
        return res;
    }
};

class StringSztMap : public Map<String *, size_t>
{
public:
    Lock lock_;

    void put(String *s, size_t n)
    {
        lock_.lock();
        add(s, n);
        lock_.unlock();
    }

    size_t retrieve(String *s)
    {
        lock_.lock();
        size_t res = get(s);
        lock_.unlock();
        return res;
    }
};

class PseudoNetwork : public Object
{
public:
    StringSztMap threads_;
    FastArray<MessageQueue *> qs_;

    PseudoNetwork(size_t num_threads) : Object()
    {
        for (size_t i = 0; i < num_threads; i++)
        {
            qs_.push_back(new MessageQueue());
        }
    }

    /**
     * @brief Map idx to the current thread
     * 
     * @param idx 
     */
    void register_node(size_t idx)
    {
        String *tid = Thread::thread_id();
        threads_.put(tid, idx);
    }

    /**
     * @brief Morally we delete the message msg on the sender side and create a new message on the receiver side
     * 
     * @param m 
     */
    void send_m(Message *m)
    {
        qs_.get(m->target_)->push(m);
    }

    Message *recv_m()
    {
        String *tid = Thread::thread_id();
        size_t i = threads_.get(tid);
        delete tid;
        return qs_.get(i)->pop();
    }
};