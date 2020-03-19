#pragma once

#include "../utils/string.h"

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
        decode(serialized)
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
        // casting everything as a char array
        StrBuff encoded_ = StrBuff();
        encoded_.c(reinterpret_cast<char *>(home)).c(name->c_str());

        // converting StrBuff to char*
        String *encodedStr = encoded_.get();
        char *serialized_ = new char[encodedStr->size()];
        serialized_ = encodedStr->c_str();
        delete encodedStr;

        return serialized_;
    }

    void decode(char *serialized)
    {
        size_t sz_size = sizeof(size_t);
        char *sz_temp = new char[sz_size];

        // deserializing the home node
        for (int i = 0; i < sz_size; i++)
        {
            sz_temp[i] = serialized[i];
        }
        home = *reinterpret_cast<size_t *>(sz_temp);

        // deserializing the name/key value
        StrBuff key_ = StrBuff();
        for (int i = sz_size; i < strlen(serialized); i++)
        {
            key_.c(serialized[i]);
        }
        name = key_.get();
    }
};