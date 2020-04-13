//lang::Cpp

#include "schema.h"
#include "../utils/thread.h"
#include "../app/reader-writer.h"

#pragma once

/**
 * @author BK and AT
 * 
 */

/*****************************************************************************
 * @brief Fielder::
 * A field vistor invoked by Row. 
 * 
 * Here for testing purposes -- see Printer
 */
class Fielder : public Object
{
public:
    /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
    virtual void start(size_t r) {}

    /** Called for fields of the argument's type with the value of the field. */
    virtual void accept(bool b) {}

    virtual void accept(float f) {}

    virtual void accept(int i) {}

    virtual void accept(String *s) {}

    /** Called when all fields have been seen. */
    virtual void done() {}
};

class Printer : public Fielder
{
public:
    size_t row_num_;
    /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
    virtual void start(size_t r) { row_num_ = r; }

    /** Called for fields of the argument's type with the value of the field. */
    virtual void accept(bool b)
    {
        p(b);
        p('\t');
    }
    virtual void accept(float f)
    {
        p(f);
        p('\t');
    }
    virtual void accept(int i)
    {
        p(i);
        p('\t');
    }
    virtual void accept(String *s)
    {
        p(s->c_str());
        p('\t');
    }

    /** Called when all fields have been seen. */
    virtual void done() { pln(""); }
};

/*************************************************************************
 * @brief Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 */
class Row : public Object
{
public:
    Schema *schema_;
    int *int_arr_;
    float *float_arr_;
    String **str_arr_;
    bool *bool_arr_;
    size_t idx_;

    /** Build a row following a schema. */
    Row(Schema &scm) : Object()
    {
        schema_ = new Schema(scm);
        size_t ncols = schema_->width();
        int_arr_ = new int[ncols];
        bool_arr_ = new bool[ncols];
        float_arr_ = new float[ncols];
        str_arr_ = new String *[ncols];
    }

    ~Row()
    {
        delete schema_;
        delete[] int_arr_;
        delete[] float_arr_;
        delete[] str_arr_;
        delete[] bool_arr_;
    }

    /** Setters: set the given column with the given value. Setting a column with
    * a value of the wrong type is undefined. */
    void set(size_t col, int val) { int_arr_[col] = val; }
    void set(size_t col, float val) { float_arr_[col] = val; }
    void set(size_t col, bool val) { bool_arr_[col] = val; }
    /** The string is external. */
    void set(size_t col, String *val) { str_arr_[col] = val->clone(); }

    /** Set/get the index of this row (ie. its position in the dataframe. 
     * This is only used for informational purposes, unused otherwise */
    void set_idx(size_t idx) { idx_ = idx; }
    size_t get_idx() { return idx_; }

    /** Getters: get the value at the given column. If the column is not
    * of the requested type, the result is undefined. */
    int get_int(size_t col) { return int_arr_[col]; }
    bool get_bool(size_t col) { return bool_arr_[col]; }
    float get_float(size_t col) { return float_arr_[col]; }
    String *get_string(size_t col) { return str_arr_[col]->clone(); }

    /** Number of fields in the row. */
    size_t width() { return schema_->width(); }

    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) { return schema_->col_type(idx); }

    /** Given a Fielder, visit every field of this row. The first argument is
    * index of the row in the dataframe.
    * Calling this method before the row's fields have been set is undefined. */
    void visit(size_t idx, Fielder &f)
    {
        f.start(idx);
        for (size_t i = 0; i < schema_->width(); i++)
        {
            if (col_type(i) == 'S')
                f.accept(str_arr_[i]);
            else if (col_type(i) == 'I')
                f.accept(int_arr_[i]);
            else if (col_type(i) == 'F')
                f.accept(float_arr_[i]);
            else if (col_type(i) == 'B')
                f.accept(bool_arr_[i]);
            else
                assert(false);
        }
        f.done();
    }
};

/*******************************************************************************
 *  @brief Rower::
 *  An interface for iterating through each row of a data frame. The intent
 *  is that this class should subclassed and the accept() method be given
 *  a meaningful implementation. Rowers can be cloned for parallel execution.
 * 
 *  @author BK and AT
 */
class Rower : public Object
{
public:
    /** This method is called once per row. The row object is on loan and
      should not be retained as it is likely going to be reused in the next
      call. The return value is used in filters to indicate that a row
      should be kept. */
    virtual bool accept(Row &r) = 0;
};

class RowPrinter : public Rower
{
public:
    /** This method is called once per row. The row object is on loan and
      should not be retained as it is likely going to be reused in the next
      call. The return value is used in filters to indicate that a row
      should be kept. */
    virtual bool accept(Row &r)
    {
        Printer printer_ = Printer();
        printer_.start(r.get_idx());
        r.visit(r.get_idx(), printer_);
        return true;
    }
};

class kvstore;
class Key;
class PseudoNetwork;

/****************************************************************************
 * @brief DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 * 
 * @author BK and AT
 */
class DataFrame
{
public:
    Schema *schema_;
    size_t nrows_;
    size_t ncols_;
    FastArray<Column *> *col_arr_;

    /** Create a data frame with the same columns as the given df but with no rows or rownames */
    DataFrame(DataFrame &df) : DataFrame(*df.schema_) {}

    /** Create a data frame from a schema and columns. All columns are created
    * empty. */
    DataFrame(Schema &schema)
    {
        schema_ = new Schema(schema);
        nrows_ = 0;
        ncols_ = schema_->width();
        col_arr_ = new FastArray<Column *>();
        for (size_t i = 0; i < ncols_; i++)
        {
            if (schema_->col_type(i) == 'S')
                col_arr_->push_back(new StringColumn());
            else if (schema_->col_type(i) == 'B')
                col_arr_->push_back(new BoolColumn());
            else if (schema_->col_type(i) == 'F')
                col_arr_->push_back(new FloatColumn());
            else if (schema_->col_type(i) == 'I')
                col_arr_->push_back(new IntColumn());
            else
                exit(1);
        }
    }

    /**
     * @brief Destroy the Data Frame object
     * 
     */
    ~DataFrame()
    {
        delete schema_;
        delete[] col_arr_;
    }

    /** Returns the dataframe's schema. Modifying the schema after a dataframe
    * has been created in undefined. */
    Schema &get_schema() { return *schema_; }

    /** Adds a column this dataframe, updates the schema, the new column
    * is external, and appears as the last column of the dataframe, the
    * name is optional and external. A nullptr column is undefined. */
    void add_column(Column *col)
    {
        assert(col != nullptr);
        if (ncols_ > 0)
            assert(col->size() == nrows_); // checking that size of column is the same
        if (ncols_ == 0)
            nrows_ = col->size(); // if there were no columns, then set the row size to this column
        col_arr_->push_back(col->clone());
        schema_->add_column(col->get_type());
        ncols_ = ncols_ + 1; // update ncol size
    }

    /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
    int get_int(size_t col, size_t row)
    {
        assert(schema_->col_type(col) == 'I');
        assert(row < nrows_);
        // gets the Column -> makes it an IntColumn -> gets the value from the Int Column
        return col_arr_->get(col)->as_int()->get(row);
    }
    bool get_bool(size_t col, size_t row)
    {
        assert(schema_->col_type(col) == 'B');
        assert(row < nrows_);
        return col_arr_->get(col)->as_bool()->get(row);
    }
    float get_float(size_t col, size_t row)
    {
        assert(schema_->col_type(col) == 'F');
        assert(row < nrows_);
        return col_arr_->get(col)->as_float()->get(row);
    }
    String *get_string(size_t col, size_t row)
    {
        assert(schema_->col_type(col) == 'S');
        assert(row < nrows_);
        return col_arr_->get(col)->as_string()->get(row);
    }

    /** Set the value at the given column and row to the given value.
    * If the column is not  of the right type or the indices are out of
    * bound, the result is undefined. */
    void set(size_t col, size_t row, int val)
    {
        assert(schema_->col_type(col) == 'I');
        assert(row < nrows_);
        col_arr_->get(col)->as_int()->set(row, val);
    }
    void set(size_t col, size_t row, bool val)
    {
        assert(schema_->col_type(col) == 'B');
        assert(row < nrows_);
        col_arr_->get(col)->as_bool()->set(row, val);
    }
    void set(size_t col, size_t row, float val)
    {
        assert(schema_->col_type(col) == 'F');
        assert(row < nrows_);
        col_arr_->get(col)->as_float()->set(row, val);
    }
    void set(size_t col, size_t row, String *val)
    {
        assert(schema_->col_type(col) == 'S');
        assert(row < nrows_);
        col_arr_->get(col)->as_string()->set(row, val);
    }

    /** Set the fields of the given row object with values from the columns at
    * the given offset.  If the row is not from the same schema as the
    * dataframe, results are undefined.
    */
    void fill_row(size_t idx, Row &row)
    {
        // check if the row has the same schema (i.e. same col type/name)
        // if the schemas are not the same then the program should terminate
        assert(schema_->width() == row.schema_->width());
        for (size_t i = 0; i < schema_->width(); i++)
        {
            assert(schema_->col_type(i) == row.schema_->col_type(i));
        }
        row.set_idx(idx);
        // adding the fields to the rows
        for (size_t i = 0; i < col_arr_->size(); i++)
        {
            if (col_arr_->get(i)->get_type() == 'S')
                row.set(i, col_arr_->get(i)->as_string()->get(idx));
            else if (col_arr_->get(i)->get_type() == 'I')
                row.set(i, col_arr_->get(i)->as_int()->get(idx));
            else if (col_arr_->get(i)->get_type() == 'F')
                row.set(i, col_arr_->get(i)->as_float()->get(idx));
            else if (col_arr_->get(i)->get_type() == 'B')
                row.set(i, col_arr_->get(i)->as_bool()->get(idx));
            else
                exit(1);
        }
    }

    /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undefined.  */
    void add_row(Row &row)
    {
        // check if the row has the same schema (i.e. same col type/name)
        // if the schemas are not the same then the program should terminate
        assert(schema_->width() == row.schema_->width());
        for (size_t i = 0; i < schema_->width(); i++)
        {
            assert(schema_->col_type(i) == row.schema_->col_type(i));
        }
        for (size_t i = 0; i < row.width(); i++)
        {
            if (row.col_type(i) == 'S')
                col_arr_->get(i)->push_back(row.get_string(i));
            else if (row.col_type(i) == 'F')
                col_arr_->get(i)->push_back(row.get_float(i));
            else if (row.col_type(i) == 'I')
                col_arr_->get(i)->push_back(row.get_int(i));
            else if (row.col_type(i) == 'B')
                col_arr_->get(i)->push_back(row.get_bool(i));
        }
        nrows_ = nrows_ + 1;
    }

    /** The number of rows in the dataframe. */
    size_t nrows() { return nrows_; }

    /** The number of columns in the dataframe.*/
    size_t ncols() { return ncols_; }

    /** Visit rows in order */
    void map(Rower &r)
    {
        Row row_ = Row(get_schema());
        for (size_t i = 0; i < nrows_; i++)
        {
            fill_row(i, row_);
            r.accept(row_);
        }
    }

    void local_map(Reader &r)
    {
        Row row_ = Row(get_schema());
        for (size_t i = 0; i < nrows_; i++)
        {
            fill_row(i, row_);
            r.visit(row_);
        }
    }

    /** Print the dataframe in SoR format to standard output. */
    void print()
    {
        RowPrinter r = RowPrinter();
        map(r);
    }

    static DataFrame *fromArray(Key *key, kvstore *kv, size_t sz, float *vals);

    static DataFrame *fromScalar(Key *key, kvstore *kv, float sum);

    static DataFrame *fromVisitor(Key *key, kvstore *kvstore, const char *schema, Writer &w);
};