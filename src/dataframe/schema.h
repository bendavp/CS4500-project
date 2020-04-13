//lang:Cwc

#include "column.h"

#pragma once

/*************************************************************************
 * @brief Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 * 
 * @author BK and AT
 */
class Schema : public Object
{
public:
    String *coltypes_;
    size_t width_;

    /** Copying constructor */
    Schema(Schema &from)
    {
        coltypes_ = from.coltypes_->clone();
        width_ = from.width_;
    }

    /** Create an empty schema **/
    Schema()
    {
        coltypes_ = new String("");
        width_ = 0;
    }

    /** Create a schema from a string of types. A string that contains
    * characters other than those identifying the four type results in
    * undefined behavior. The argument is external, a nullptr argument is
    * undefined. **/
    Schema(const char *types)
    {
        assert(types != nullptr);
        coltypes_ = new String(types);
        width_ = coltypes_->size();
    }

    ~Schema() { delete coltypes_; }

    /** Add a column of the given type and name (can be nullptr), name
    * is external. Names are expectd to be unique, duplicates result
    * in undefined behavior. */
    void add_column(char typ)
    {
        char *c = new char(typ);
        coltypes_ = StrBuff().c(coltypes_->c_str()).c(c, 1).get();
        delete c;
        width_++;
    }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx)
    {
        assert(idx < width());
        return coltypes_->at(idx);
    }

    /** The number of columns */
    size_t width() { return width_; }
};