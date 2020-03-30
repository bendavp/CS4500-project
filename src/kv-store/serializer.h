#pragma once

#include <string.h>
#include "key.h"

// based of our understanding from link below:
// http://www.cplusplus.com/forum/general/201675/
class Serializer
{
public:
    Serializer() {}

    void *serialize_key(Key *key, char *buffer)
    {
        StrBuff sb_ = StrBuff();
        char *temp_buff = new char[sizeof(size_t)];
        serialize_size_t(key->home, temp_buff);
        sb_.c(temp_buff, sizeof(size_t));
        delete[] temp_buff;

        temp_buff = new char[key->name->size()];
        serialize_String(key->name, temp_buff);
        sb_.c(temp_buff, key->name->size());
        delete[] temp_buff;

        temp_buff = new char[1];
        temp_buff[0] = '\t';
        sb_.c(temp_buff, 1);

        String *temp_ = sb_.get();
        buffer = temp_->steal();
        delete temp_;
    }

    Key *deserialize_key(char *key_buffer)
    {
        char *temp_buffer = new char[sizeof(size_t)];
        for (int i = 0; i < sizeof(size_t); i++)
        {
            temp_buffer[i] = key_buffer[i];
        }

        size_t home = deserialize_size_t(temp_buffer);
        delete[] temp_buffer;

        StrBuff sb_ = StrBuff();

        int i = sizeof(size_t);
        temp_buffer = new char[1];
        while (key_buffer[i] != '\t')
        {
            temp_buffer[0] = key_buffer[i];
            sb_.c(temp_buffer, 1);
            i++;
        }

        String *name = sb_.get();
        return new Key(name, home);
    }

    void serialize_size_t(size_t val, char *buffer)
    {
        memcpy(buffer, &val, sizeof(size_t));
    }

    size_t deserialize_size_t(char *size_t_buffer)
    {
        size_t val;
        memcpy(&val, size_t_buffer, sizeof(size_t));
        return val;
    }

    void serialize_int(int val, char *buffer)
    {
        memcpy(buffer, &val, sizeof(int));
    }

    int deserialize_int(char *int_buffer)
    {
        int val;
        memcpy(&val, int_buffer, sizeof(int));
        return val;
    }

    void serialize_bool(bool val, char *buffer)
    {
        serialize_int((int)val, buffer);
    }

    bool deserialize_bool(char *bool_buffer)
    {
        bool val = (bool)deserialize_int(bool_buffer);
        return val;
    }

    void serialize_float(float val, char *buffer)
    {
        memcpy(buffer, &val, sizeof(float));
    }

    float deserialize_float(char *float_buffer)
    {
        float val;
        memcpy(&val, float_buffer, sizeof(float));
        return val;
    }

    void serialize_String(String *val, char *buffer)
    {
        StrBuff sb_ = StrBuff();
        size_t val_sz = val->size();
        memcpy(buffer, val->c_str(), val_sz);
    }

    String *deserialize_String(char *string_buffer)
    {
        return new String(string_buffer);
    }

    // for testing purposes
    void serialize_StringArray(String **strArray, char *buffer, size_t sz)
    {
        StrBuff sb_ = StrBuff();
        char *sep = new char[1];
        sep[0] = '\t';
        String *s_temp;
        for (size_t i = 0; i < sz; i++)
        {
            s_temp = strArray[i]->clone();
            sb_.c(s_temp->c_str(), s_temp->size()).c(sep, 1);
            delete s_temp;
        }
        String *s_ = sb_.get();
        size_t s_sz = s_->size();
        memcpy(buffer, s_->steal(), s_sz);
        delete s_;
    }

    // for testing purposes
    String **deserialize_StringArray(char *str_buffer, size_t sz)
    {
        int current = 0;
        char sep = '\t';
        String **strArray = new String *[sz];
        StrBuff sb_;
        char *toAdd = new char[1];
        for (int i = 0; i < sz; i++)
        {
            sb_ = StrBuff();
            int j = current;
            while (str_buffer[j] != sep)
            {
                toAdd[0] = str_buffer[j];
                sb_.c(toAdd, 1);
                j++;
            }
            current = j + 1;
            strArray[i] = sb_.get();
        }
        return strArray;
    }
};