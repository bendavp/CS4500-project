#include "../dataframe/dataframe.h"
#include "serializer.h"

#pragma once

enum class ValueKind
{
    Unassigned,
    DataFrame,
    Keys
};

class Value : public Object
{
public:
    char *serialized_;
    char sep = '\t';
    ValueKind vk;

    Value() : Object()
    {
        serialized_ = nullptr;
        vk = ValueKind::Unassigned;
    }

    Value(ValueKind vk_, char *serialized) : Object()
    {
        vk = vk_;
        serialized_ = serialized;
    }

    Value(DataFrame *df) : Value()
    {
        encode(df);
    }

    Value(Key **keys, size_t sz) : Value()
    {
        encode(keys, sz);
    }

    void encode(Key **keys, size_t numKeys)
    {
        assert(serialized_ == nullptr);
        assert(vk == ValueKind::Unassigned);

        // setting the type to be key array
        vk = ValueKind::Keys;

        StrBuff builder = StrBuff();
        Serializer serializer_ = Serializer();

        // storing size of the key array
        char *buffer = new char[sizeof(size_t)];
        serializer_.serialize_size_t(numKeys, buffer);
        builder.c(buffer, sizeof(size_t));
        delete[] buffer;

        // encoding the keys
        char *sep_ = new char[1];
        sep_[0] = sep;
        for (size_t i = 0; i < numKeys; i++)
        {
            size_t size_ = keys[i]->name->size() + 1 + sizeof(size_t);
            buffer = new char[size_];
            serializer_.serialize_key(keys[i], buffer);
            builder.c(buffer, size_);
            delete[] buffer;
        }
        String *encodedStr = builder.get();
        serialized_ = encodedStr->steal();
        delete encodedStr;
    }

    /**
     * @brief encodes the given DataFrame into a char* and stores the encoded DataFrame in serialized_
     * 
     * @param df -- DataFrame to be encoded
     */
    void encode(DataFrame *df)
    {
        assert(serialized_ == nullptr);
        assert(vk == ValueKind::Unassigned);

        // setting the type to be dataframe chunk
        vk = ValueKind::DataFrame;

        Serializer serializer_ = Serializer();
        StrBuff builder = StrBuff();

        // adding num of columns
        int col = df->ncols();
        char *buffer = new char[sizeof(int)];
        serializer_.serialize_int(col, buffer);
        builder.c(buffer, sizeof(int));
        delete[] buffer;
        // adding num of rows
        int row = df->nrows();
        buffer = new char[sizeof(int)];
        serializer_.serialize_int(row, buffer);
        builder.c(buffer, sizeof(int));
        delete[] buffer;

        // adding the col-types into the encoded
        String *schema_coltypes = df->get_schema().coltypes_->clone();
        builder.c(schema_coltypes->c_str());

        // adding the dataframe information column by column
        // iterating through df by col then row
        for (int i = 0; i < col; i++)
        {
            // bools
            if (schema_coltypes->at(i) == 'B')
            {
                for (int j = 0; j < row; j++)
                {
                    bool b_ = df->get_bool(i, j);
                    char *buffer = new char[sizeof(int)];
                    serializer_.serialize_bool(b_, buffer);
                    builder.c(buffer, sizeof(int));
                    delete[] buffer;
                }
            }
            // ints
            else if (schema_coltypes->at(i) == 'I')
            {
                for (int j = 0; j < row; j++)
                {
                    int i_ = df->get_int(i, j);
                    char *buffer = new char[sizeof(int)];
                    serializer_.serialize_int(i_, buffer);
                    builder.c(buffer, sizeof(int));
                    delete[] buffer;
                }
            }
            // floats
            else if (schema_coltypes->at(i) == 'F')
            {
                for (int j = 0; j < row; j++)
                {
                    float f_ = df->get_float(i, j);
                    char *buffer = new char[sizeof(float)];
                    serializer_.serialize_float(f_, buffer);
                    builder.c(buffer, sizeof(float));
                    delete[] buffer;
                }
            }
            // strings
            else if (schema_coltypes->at(i) == 'S')
            {
                String *s_temp;
                char *buffer;
                char *sep_ = new char[1];
                sep_[0] = sep;
                for (int j = 0; j < row; j++)
                {
                    s_temp = df->get_string(i, j);
                    size_t s_sz = s_temp->size();
                    buffer = new char[s_sz];
                    serializer_.serialize_String(s_temp, buffer);
                    builder.c(buffer, s_sz).c(sep_, 1);
                }
            }
        }

        // creating the char* from the StrBuff
        String *encodedStr = builder.get();
        serialized_ = encodedStr->steal();

        // deleting created objects
        delete encodedStr;
        delete schema_coltypes;
    }

    Key **decode_keys()
    {
        assert(serialized_ != nullptr);
        Serializer serializer_ = Serializer();

        size_t current = 0; // the head of the serialized string
        char *buffer = new char[sizeof(size_t)];
        for (size_t i = 0; i < sizeof(size_t); i++)
        {
            buffer[i] = serialized_[i];
            current++;
        }
        size_t numKeys = serializer_.deserialize_size_t(buffer);
        delete[] buffer;

        Key **keys = new Key *[numKeys];
        StrBuff sb_ = StrBuff();
        char *toAdd = new char[1];
        String *temp;
        for (size_t i = 0; i < numKeys; i++)
        {
            while (serialized_[current] != '\t')
            {
                toAdd[0] = serialized_[current];
                sb_.c(toAdd, 1);
                current++;
            }
            toAdd[0] = serialized_[current]; // reappends the /t needed for deserialization of key
            sb_.c(toAdd, 1);
            temp = sb_.get();
            buffer = temp->steal();
            delete temp;
            keys[i] = serializer_.deserialize_key(buffer); // deserializing and adding the key to the array
            delete[] buffer;
        }
    }

    /**
     * @brief Decodes the serialization_ into a DataFrame
     * 
     * @return DataFrame* 
     */
    DataFrame *decode_df()
    {
        assert(serialized_ != nullptr);
        Serializer serializer_ = Serializer();

        // size of the different primitives when encoded
        size_t int_size = sizeof(int);
        size_t bool_size = sizeof(int);
        size_t float_size = sizeof(float);
        // creating temp char array to hold info before deserializing
        char *int_temp = new char[int_size];
        char *bool_temp = new char[bool_size];
        char *float_temp = new char[float_size];

        size_t current = 0; // keep track of how far along we have moved

        // getting the total num of columns
        for (int i = current; i < int_size; i++)
        {
            int_temp[i] = serialized_[i];
            current++;
        }
        int col = serializer_.deserialize_int(int_temp);
        delete[] int_temp;
        int_temp = new char[int_size];

        // getting the total num of rows
        int current2 = current;
        for (int i = current2; i < current2 + int_size; i++)
        {
            int_temp[i - current2] = serialized_[i];
            current++;
        }
        int row = serializer_.deserialize_int(int_temp);

        // getting the col types
        char *ct_ = new char[col];
        current2 = current;
        for (int i = current2; i < current2 + col; i++)
        {
            ct_[i - current2] = serialized_[i];
            current++;
        }
        String *col_types = new String(ct_);

        Schema *scm = new Schema();
        DataFrame *decoded_ = new DataFrame(*scm); // empty dataframe that will be filled

        for (int i = 0; i < col; i++)
        {
            // adding a bool column
            if (col_types->at(i) == 'B')
            {
                // initializing empty bool column
                Column *bc_ = new BoolColumn();
                // iterating through serialized and adding each deserialized bool to the column
                for (int i = 0; i < row; i++)
                {
                    current2 = current;
                    for (int j = current2; j < current2 + bool_size; j++)
                    {
                        bool_temp[j - current2] = serialized_[j];
                        current++;
                    }
                    bool b_ = serializer_.deserialize_bool(bool_temp);
                    bc_->push_back(b_);
                }
                // adding copy of bool column to the dataframe
                decoded_->add_column(bc_->clone(), nullptr);
                delete bc_; // delete to reset
            }
            // adding an int column
            else if (col_types->at(i) == 'I')
            {
                // initializing empty int column
                Column *ic_ = new IntColumn();
                // iterating through serialized and adding each deserialized int to the column
                for (int i = 0; i < row; i++)
                {
                    current2 = current;
                    for (int j = current2; j < current2 + int_size; j++)
                    {
                        int_temp[j - current2] = serialized_[j];
                        current++;
                    }
                    int i_ = serializer_.deserialize_int(int_temp);
                    ic_->push_back(i_);
                }
                // adding copy of int column to the dataframe
                decoded_->add_column(ic_->clone(), nullptr);
                delete ic_; // delete to reset
            }
            // adding a float column
            else if (col_types->at(i) == 'F')
            {
                // initializing empty float column
                Column *fc_ = new FloatColumn();
                // iterating through serialized and adding each deserialized float to the column
                for (int i = 0; i < row; i++)
                {
                    current2 = current;
                    for (int j = current2; j < current2 + float_size; j++)
                    {
                        float_temp[j - current2] = serialized_[j];
                        current++;
                    }
                    float f_ = serializer_.deserialize_float(float_temp);
                    fc_->push_back(f_);
                }
                // adding copy of float column to the dataframe
                decoded_->add_column(fc_->clone(), nullptr);
                delete fc_; // delete to reset
            }
            // adding a String column
            else if (col_types->at(i) == 'S')
            {
                // initializing new string column
                Column *sc_ = new StringColumn();
                StrBuff sb_; // strbuff will be used to build each string
                char *toAdd = new char[1];
                // iterating through serialized to build strings and add the built strings to the string column
                for (int i = 0; i < row; i++)
                {
                    sb_ = StrBuff();
                    int j = current;
                    // continuing to iterate through and add the character to the strbuff until we see the separator ('/t')
                    while (serialized_[j] != sep)
                    {
                        toAdd[0] = serialized_[j];
                        sb_.c(toAdd, 1);
                        j++;
                    }
                    current = j + 1; // need to set the head to the character after the separator (otherwise an empty string will be added)
                    sc_->push_back(sb_.get());
                }
                // adding copy of string column to the dataframe
                decoded_->add_column(sc_->clone(), nullptr);
                delete sc_; // delete to reset
            }
        }
        delete col_types;
        return decoded_;
    }

    bool equals(Object *other)
    {
        Value *other_val = dynamic_cast<Value *>(other);
        if (other_val == nullptr)
            return false;
        if (other_val->serialized_ == nullptr)
        {
            if (serialized_ != nullptr)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        return strcmp(other_val->serialized_, serialized_) == 0;
    }

    // hashes the serialization as if it is a string object
    size_t hash()
    {
        return String(serialized_).hash();
    }

    Value *clone()
    {
        size_t size = strlen(serialized_);
        char *temp_ = new char[size];
        memcpy(temp_, serialized_, size);
        Value *res = new Value(vk, temp_);
        return res;
    }
};