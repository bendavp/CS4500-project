#include "../dataframe/dataframe.h"
#include "serializer.h"

#pragma once

/**
 * @brief 
 * 
 */
class Value : public Object
{
public:
    char *serialized_;
    char sep = '\t';

    Value() : Object()
    {
        serialized_ = nullptr;
    }

    Value(DataFrame *df) : Object()
    {
        encode(df);
    }

    /**
     * @brief encodes the given DataFrame into a char* 
     * 
     * @param df -- DataFrame to be encoded
     */
    void encode(DataFrame *df)
    {
        Serializer serializer_ = Serializer();
        StrBuff builder = StrBuff();

        // adding num of columns
        int col = df->ncols();
        std::cout << col << std::endl;
        char *buffer = new char[sizeof(int)];
        serializer_.serialize_int(col, buffer);
        builder.c(buffer, sizeof(int));
        delete[] buffer;
        // adding num of rows
        int row = df->nrows();
        std::cout << row << std::endl;
        buffer = new char[sizeof(int)];
        serializer_.serialize_int(row, buffer);
        builder.c(buffer, sizeof(int));
        delete[] buffer;

        // adding the col-types into the encoded
        String *schema_coltypes = df->get_schema().coltypes_;
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
            // floats (casted into a double first due to inability to just cast float into char*)
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

    /**
     * @brief Decodes the serialization_ into a DataFrame
     * 
     * @return DataFrame* 
     */
    DataFrame *decode()
    {
        Serializer serializer_ = Serializer();
        std::cout << "im decoding now\n";
        assert(serialized_ != nullptr);
        std::cout << "im decoding now\n";
        // size_t encoded_size = strlen(serialized_); // make sure we don't go past how many bytes we serialized
        // assert(encoded_size != 0);

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

        std::cout << "col and row are: " << col << " " << row << '\n';

        // getting the col types
        // StrBuff sb_;
        char *ct_ = new char[col];
        current2 = current;
        for (int i = current2; i < current2 + col; i++)
        {
            // sb_.c(serialized_[i]);
            ct_[i - current2] = serialized_[i];
            current++;
        }
        // String *col_types = sb_.get();
        String *col_types = new String(ct_);
        std::cout << "col types here " << col_types->c_str() << '\n';

        std::cout << "here\n";

        Schema *scm = new Schema();
        DataFrame *decoded_ = new DataFrame(*scm); // empty dataframe that will be filled

        for (int i = 0; i < col; i++)
        {
            // adding a bool column
            if (col_types->at(i) == 'B')
            {
                Column *bc_ = new BoolColumn();
                for (int i = 0; i < row; i++)
                {
                    current2 = current;
                    for (int j = current2; j < current2 + bool_size; j++)
                    {
                        bool_temp[j - current2] = serialized_[j];
                        current++;
                    }
                    bool b_ = serializer_.deserialize_bool(bool_temp);
                    std::cout << b_ << std::endl;
                    bc_->push_back(b_);
                }
                decoded_->add_column(bc_->clone(), nullptr);
                delete bc_;
            }
            // adding an int column
            else if (col_types->at(i) == 'I')
            {
                Column *ic_ = new IntColumn();
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
                decoded_->add_column(ic_->clone(), nullptr);
                delete ic_;
            }
            // adding a float column
            else if (col_types->at(i) == 'F')
            {
                Column *fc_ = new FloatColumn();
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
                decoded_->add_column(fc_->clone(), nullptr);
                delete fc_;
            }
            // adding a String column
            else if (col_types->at(i) == 'S')
            {
                Column *sc_ = new StringColumn();
                StrBuff sb_;
                char *toAdd = new char[1];
                for (int i = 0; i < row; i++)
                {
                    sb_ = StrBuff();
                    int j = current;
                    while (serialized_[j] != sep)
                    {
                        toAdd[0] = serialized_[j];
                        sb_.c(toAdd, 1);
                        j++;
                    }
                    current = j + 1;
                    sc_->push_back(sb_.get());
                }
                decoded_->add_column(sc_->clone(), nullptr);
                delete sc_;
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
        return strcmp(other_val->serialized_, serialized_) == 0;
    }

    // hashes the serialization as if it is a string object
    size_t hash()
    {
        return String(serialized_).hash();
    }
};