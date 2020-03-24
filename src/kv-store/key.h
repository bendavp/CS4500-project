#pragma once

#include "../utils/string.h"
#include "serializer.h"

class Key : public Object
{
public:
    String *name; // unique identifier for the key
    size_t home;  // home node on which this key lives

    Key() : Object()
    {
        name = new String("key");
        home = 0;
    }

    Key(size_t h) : Object()
    {
        name = new String("key");
        home = h;
    }

    Key(String *n) : Object()
    {
        name = n;
        home = 0;
    }

    Key(String *n, size_t h) : Object()
    {
        name = n;
        home = h;
    }

    Key(char *serialized) : Object()
    {
        decode(serialized);
    }

    bool equals(Object *other)
    {
        Key *other_key = dynamic_cast<Key *>(other);
        if (other_key == nullptr)
            return false;
        return other_key->name->equals(name) && other_key->home == home;
    }

    size_t hash()
    {
        return name->hash();
    }

    char *encode()
    {
        Serializer s_ = Serializer();
        StrBuff sb_ = StrBuff();
        char *buffer = new char[sizeof(size_t)];
        s_.serialize_size_t(home, buffer);
        sb_.c(buffer, sizeof(size_t));
        delete[] buffer;

        buffer = new char[name->size()];
        s_.serialize_String(name, buffer);
        sb_.c(buffer, name->size());
        delete[] buffer;

        buffer = new char[1];
        buffer[0] = '\t';
        sb_.c(buffer, 1);

        String *temp_ = sb_.get();
        char *serialized_ = temp_->steal();
        delete temp_;

        return serialized_;
    }

    void decode(char *serialized)
    {
        Serializer s_ = Serializer();

        char *temp_buffer = new char[sizeof(size_t)];
        for (int i = 0; i < sizeof(size_t); i++)
        {
            temp_buffer[i] = serialized[i];
        }

        home = s_.deserialize_size_t(temp_buffer);
        delete[] temp_buffer;

        StrBuff sb_ = StrBuff();

        int i = sizeof(size_t);
        temp_buffer = new char[1];
        while (serialized[i] != '\t')
        {
            temp_buffer[0] = serialized[i];
            sb_.c(temp_buffer, 1);
            i++;
        }

        name = sb_.get();
    }
};