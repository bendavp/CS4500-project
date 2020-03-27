
MQArray(size_t cap) : Array(cap) {}
MessageQueue *get(size_t i) { return dynamic_case<MessageQueue *>(get_(i)); }

extern Args arg;

class NetworkPseudo : public NetworkIfc
{
public:
    String_size_t_Map threads_;
    MQArray qs_;

    NetworkPseudo() : qs_(1)
    {
        for (size_t i = 0; i < arg.num_nodes; i++)
        {
            qs_.push_back(new MessageQueue());
        }
    }

    /**
     * @brief Map idx to the current thread
     * 
     * @param idx 
     */
    void register_node(size_t idx) override
    {
        String *tid = Thread::thread_id();
        threads_.set_u(*tid, idx);
        delete tid;
    }

    /**
     * @brief Morally we delete the message msg on the sender side and create a new message on the receiver side
     * 
     * @param m 
     */
    void send_m(Message *m) override
    {
        qs_get(msg->target_)->push(msg);
    }

    Message *recv_m() override
    {
        String *tid = Thread::thread_id();
        size_t i = threads_.get(*tid);
        delete tid;
        return qs_.get(i)->pop)();
    }
};