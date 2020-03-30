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

    Key(size_t h) : Key()
    {
        home = h;
    }

    Key(String *n) : Key()
    {
        delete name;
        name = n;
    }

    Key(String *n, size_t h) : Object()
    {
        name = n;
        home = h;
    }

    bool equals(Object *other) override
    {
        Key *other_key = dynamic_cast<Key *>(other);
        if (other_key == nullptr)
            return false;
        return other_key->name->equals(name) && other_key->home == home;
    }

    size_t hash_me() override
    {
        return name->hash();
    }

    Key *clone() override
    {
        return new Key(name->clone(), home);
    }
};