#include "../dataframe/dataframe.h"

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
        StrBuff builder = StrBuff();

        // adding num of columns then rows
        int col = df->ncols();
        int row = df->nrows();
        builder.c(reinterpret_cast<char *>(col)).c(reinterpret_cast<char *>(row));

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
                    builder.c(reinterpret_cast<char *>(df->get_bool(i, j)));
                }
            }
            // ints
            else if (schema_coltypes->at(i) == 'I')
            {
                for (int j = 0; j < row; j++)
                {
                    builder.c(reinterpret_cast<char *>(df->get_int(i, j)));
                }
            }
            // floats
            else if (schema_coltypes->at(i) == 'F')
            {
                for (int j = 0; j < row; j++)
                {
                    builder.c(reinterpret_cast<char *>(df->get_float(i, j)));
                }
            }
            // strings
            else if (schema_coltypes->at(i) == 'S')
            {
                for (int j = 0; j < row; j++)
                {
                    builder.c(df->get_string(i, j)->c_str()).c(sep);
                }
            }
        }

        // creating the char* from the StrBuff
        String *encodedStr = builder.get();
        char *serialized_ = new char[encodedStr->size()];
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
        assert(serialized_ != nullptr);

        // size_t encoded_size = strlen(serialized_); // make sure we don't go past how many bytes we serialized
        // assert(encoded_size != 0);

        // size of the different primitives when encoded
        size_t int_size = sizeof(int);
        size_t bool_size = sizeof(bool);
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
        int col = (int)int_temp;

        // getting the total num of rows
        for (int i = current; i < int_size; i++)
        {
            int_temp[i] = serialized_[i];
            current++;
        }
        int row = (int)int_temp;

        // getting the col types
        StrBuff sb_;
        for (int i = current; i < col; i++)
        {
            sb_.c(i);
            current++;
        }
        String *col_types = sb_.get();

        Schema scm = Schema();
        DataFrame *decoded_ = new DataFrame(scm); // empty dataframe that will be filled

        for (int i = 0; i < col; i++)
        {
            // adding a bool column
            if (col_types->at(i) == 'B')
            {
                BoolColumn *bc_ = new BoolColumn();
                for (int i = 0; i < row; i++)
                {
                    for (int j = current; j < bool_size; j++)
                    {
                        bool_temp[j] = serialized_[j];
                        current++;
                    }
                    bc_->push_back((bool)bool_temp);
                }
                decoded_->add_column(bc_.clone(), nullptr);
                delete bc_;
            }
            // adding an int column
            else if (col_types->at(i) == 'I')
            {
                IntColumn *ic_ = new IntColumn();
                current = decode_helper(ic_, current, int_size, row);
                for (int i = 0; i < row; i++)
                {
                    for (int j = current; j < int_size; j++)
                    {
                        int_temp[j] = serialized_[j];
                        current++;
                    }
                    ic_->push_back((int)int_temp);
                }
                decoded_->add_column(ic_.clone(), nullptr);
                delete ic_;
            }
            // adding a float column
            else if (col_types->at(i) == 'F')
            {
                FloatColumn *fc_ = new FloatColumn();
                for (int i = 0; i < row; i++)
                {
                    for (int j = current; j < float_size; j++)
                    {
                        float_temp[j] = serialized_[j];
                        current++;
                    }
                    fc_->push_back((float)float_temp);
                }
                decoded_->add_column(fc_.clone(), nullptr);
                delete fc_;
            }
            // adding a String column
            else if (col_types->at(i) == 'S')
            {
                StringColumn *sc_ = new StringColumn();
                StrBuff sb_ = StrBuff();
                for (int i = 0; i < row; i++)
                {
                    int j = current;
                    while (serialized_[j] != sep)
                    {
                        sb_.c(serialized_[j]);
                        current++;
                        j++;
                    }
                    sc_->push_back(sb_.get());
                }
            }
        }
        delete col_types;
        return decoded_;
    }