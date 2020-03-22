//lang:Cwc

#include "column.h"

#pragma once
/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object
{
public:
    String *coltypes_;
    StringFastArray *row_names_;
    StringFastArray *col_names_;

    /** Copying constructor */
    Schema(Schema &from)
    {
        coltypes_ = from.coltypes_->clone();
        row_names_ = from.row_names_->clone();
        col_names_ = from.col_names_->clone();
    }

    /** Create an empty schema **/
    Schema()
    {
        coltypes_ = new String("");
        row_names_ = new StringFastArray();
        col_names_ = new StringFastArray();
    }

    /** Create a schema from a string of types. A string that contains
    * characters other than those identifying the four type results in
    * undefined behavior. The argument is external, a nullptr argument is
    * undefined. **/
    Schema(const char *types)
    {
        assert(types != nullptr);
        coltypes_ = new String(types);
        row_names_ = new StringFastArray();
        col_names_ = new StringFastArray();
        for (int i = 0; i < strlen(types); i++)
        {
            col_names_->push_back(nullptr);
        }
    }

    ~Schema()
    {
        delete row_names_;
        delete col_names_;
    }

    /** Add a column of the given type and name (can be nullptr), name
    * is external. Names are expectd to be unique, duplicates result
    * in undefined behavior. */
    void add_column(char typ, String *name)
    {
        if (name != nullptr)
        {
            // check all newly added name is different than the current names; terminates otherwise
            for (size_t i = 0; i < col_names_->size(); i++)
            {
                if (col_names_->get(i) != nullptr)
                {
                    assert(!name->equals(col_names_->get(i)));
                }
            }
        }
        char *c = new char(typ);
        std::cout << "adding col type of " << c << std::endl;
        coltypes_ = StrBuff().c(coltypes_->c_str()).c(c, 1).get();
        delete c;
        std::cout << coltypes_->c_str() << std::endl;
        col_names_->push_back(name);
    }

    /** Add a row with a name (possibly nullptr), name is external.  Names are
   *  expectd to be unique, duplicates result in undefined behavior. */
    void add_row(String *name)
    {
        if (name != nullptr)
        {
            for (size_t i = 0; i < row_names_->size(); i++)
            {
                if (row_names_->get(i) != nullptr)
                {
                    assert(!name->equals(row_names_->get(i)));
                }
            }
            // check all newly added name is different than the current names; terminates otherwise
        }
        row_names_->push_back(name);
    }

    /** Return name of row at idx; nullptr indicates no name. An idx >= width
    * is undefined. */
    String *row_name(size_t idx)
    {
        assert(idx < length());
        return row_names_->get(idx);
    }

    /** Return name of column at idx; nullptr indicates no name given.
    *  An idx >= width is undefined.*/
    String *col_name(size_t idx)
    {
        assert(idx < width());
        return col_names_->get(idx);
    }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx)
    {
        assert(idx < width());
        return coltypes_->at(idx);
    }

    /** Given a column name return its index, or -1. */
    int col_idx(const char *name)
    {
        assert(name != nullptr);
        String *name_str = new String(name);
        for (int i = 0; i < col_names_->size(); i++)
        {
            if (col_names_->get(i) != nullptr && col_names_->get(i)->equals(name_str))
            {
                delete name_str;
                return i;
            }
        }
        delete name_str;
        return -1;
    }

    /** Given a row name return its index, or -1. */
    int row_idx(const char *name)
    {
        assert(name != nullptr);
        String *name_str = new String(name);
        for (int i = 0; i < row_names_->size(); i++)
        {
            if (row_names_->get(i) != nullptr && row_names_->get(i)->equals(name_str))
            {
                delete name_str;
                return i;
            }
        }
        delete name_str;
        return -1;
    }

    /** The number of columns */
    size_t width()
    {
        return col_names_->size();
    }

    /** The number of rows */
    size_t length()
    {
        return row_names_->size();
    }
};