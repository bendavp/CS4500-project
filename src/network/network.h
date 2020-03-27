#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "message.h"

class NodeInfo : public Object
{
public:
    size_t id;
    sockaddr_in address;
};

class Network : public Object
{
public:
    NodeInfo *nodes_;
    size_t this_node_;
    int sock_;
    sockaddr_in ip;

    ~Network()
    {
        close(sock_);
    }

    size_t index()
    {
        return this_node_;
    }

    void server_init(size_t idx, int port)
    {
        this_node_ = idx;                               // set this node to given index
        init_sock_(port);                               // initialize socket of this node with given port
        nodes_ = new NodeInfo[num_nodes_from_cmd_line]; // create list of all nodes

        // initialize id of all nodes to 0
        for (int i = 0; i < num_nodes_from_cmd_line; i++)
        {
            nodes_[i].id = 0;
        }
        nodes_[0].address = ip_; // set address of current node to our current ip (not sure where we get ip from, maybe cmd line?)
        nodes_[0].id = 0;        // we are on the server node, so we set the id to 0, since that is the server's id

        // go through all the nodes (not sure why we start from 2?) and register(?) them
        for (int i = 2; i <= num_nodes_from_cmd_line; i++)
        {
            Register *msg = dynamic_cast<Register *>(recv_m());            // receive the register(?) message
            nodes_[msg->sender()].id = msg->sender();                      // set the id of the sender node
            nodes_[msg->sender()].address.sin_family = AF_INET;            // set ip protocol of sender node to IPv4
            nodes_[msg->sender()].address.sin_addr = msg->client.sin_addr; // set the sin_addr (ip) of sender to same one as client sent from
            nodes_[msg->sender()].address.sin_port = htons(msg->port);     // set port of sender to same port as the message was received on
        }
        // once all clients are registered, create ports and addresses list:
        size_t *ports = new size_t[num_nodes_from_cmd_line - 1];
        String **addresses = new String *[num_nodes_from_cmd_line - 1];
        // fill in info in ports and addresses list
        for (int i = 0; i < num_nodes_from_cmd_line - 1; i++)
        {
            ports[i] = ntohs(nodes_[i + 1].address.sin_port);
            addresses[i] = new String(inet_ntoa(nodes_[i + 1].address.sin_addr));
        }

        Directory ipd(ports, addresses); // create a directory with ports and addresses of all nodes
        // give all nodes updated directory, since a new node has connected
        for (int i = 1; i < num_nodes_from_cmd_line; i++)
        {
            ipd.target = i; // set target of directory to node we want to send the directory to
            send_m(&ipd);   // send the directory
        }
    }

    void client_init(size_t idx, int port, char *server_addr, int server_port)
    {
        this_node_ = idx;
        init_sock_(port);
        nodes_ = new NodeInfo[1];
        nodes_[0].id = 0;
        nodes_[0].address.sin_family = AF_INET;
        nodes_[0].address.sin_port = htons(server_port);
        // if the server address is wrong/not found, throw an error
        if (inet_pton(AF_INET, server_addr, &nodes_[0].address.sin_addr) <= 0)
        {
            assert(false && "Invalid server IP address format");
        }

        Register msg(idx, port); // create a register message for the server
        send_m(&msg);            // send that message

        Directory *ipd = dynamic_cast<Directory *>(recv_m());    // create a directory with directory message received from server
        NodeInfo *nodes = new NodeInfo[num_nodes_from_cmd_line]; // create list of nodes
        nodes[0] = nodes_[0];                                    // set 0th node to server

        // go through all clients in directory message and set info in client's nodes to nodes found in directory message
        for (int i = 0; i < ipd->clients; i++)
        {
            nodes[i + 1].id = i + 1;
            nodes[i + 1].address.sin_family = AF_INET;
            nodes[i + 1].address.sin_port = htons(ipd->ports[i]);
            if (inet_pton(AF_INET, ipd->addresses[i]->c_str(), &nodes[i + 1].address.sin_addr) <= 0)
            {
                // throw an error if client is not found
            }
        }
        delete[] nodes_;
        nodes_ = nodes; // replace old nodes with new ones
        delete ipd;
    }

    // create a socket and bind it
    void init_sock_(int port)
    {
        assert((sock_ = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
        int opt = 1;
        assert(setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == 0);
        ip_.sin_family = AF_INET;
        ip_.sin_addr.s_addr = INADDR_ANY;
        ip_.sin_port = htons(port);
        assert(bind(sock_, (sockaddr *)&ip_, sizeof(ip_)) >= 0);
        assert(listen(sock_, 100) >= 0); // 100 is the connections queue size
    }

    void send_m(Message *msg)
    {
        NodeInfo &tgt = nodes_[msg->target()];
        int conn = socket(AF_INET, SOCK_STREAM, 0);
        assert(conn >= 0 && "Unable to create client socket");
        if (connect(conn, (sockaddr *)&tgt.address, sizeof(tgt.address)) < 0)
        {
            // throw error "Unable to connect to remote node"
        }
        Serializer ser;
        msg->serialize(ser);
        char *buf = ser.build();
        size_t size = ser.size();
        send(conn, &size, sizeof(size_t), 0);
        send(conn, buf, size, 0);
    }

    Message *recv_m()
    {
        sockaddr_in sender;
        socklen_t addrlen = sizeof(sender);
        int req = accept(sock_, (sockaddr *)&sender, &addrlen);
        size_t size = 0;
        if (read(req, &size, sizeof(size_t)) == 0)
        {
            // throw error "failed to read"
        }
        char *buf = new char[size];
        int rd = 0;
        while (rd != size)
        {
            rd += read(req, buf + rd, size - rd);
        }
        Deserializer s(buf, size);
        Message *msg = Message::deserialize(s, sender);
        return msg;
    }
};
