#include <gtest/gtest.h>
#include "dataframe.h"

#define GT_TRUE(a) ASSERT_EQ((a), true)
#define GT_FALSE(a) ASSERT_EQ((a), false)
#define GT_EQUALS(a, b) ASSERT_EQ(a, b)
#define ASSERT_EXIT_ZERO(a) \
    ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*")
#define CS4500_ASSERT_TRUE(a) \
    ASSERT_EQ((a), true);
#define CS4500_ASSERT_FALSE(a) \
    ASSERT_EQ((a), false);
#define CS4500_ASSERT_EXIT_ZERO(a) \
    ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*");

/********************************************************************************************************************
 * testing FastArray
 * 
 **/

/**
 * @brief Tests if we can actually pass va_list into a constructor as the literal va_list.
 * This is because all Columns take a va_list and pass it into the FastArray constructors
 * 
 * Jan wasn't sure if this would actually work so we decided to test it out!
 */
void doesVaListActuallyWork()
{
    IntFastArray *i = new IntFastArray(5, 1, 2, 3, 4, 5);
    BoolFastArray *b = new BoolFastArray(2, true, false);
    FloatFastArray *f = new FloatFastArray(4, 4.20, 4.20, 4.20, 4.20);
    StringFastArray *s = new StringFastArray(2, new String("hello"), new String("world"));

    IntColumn *ic = new IntColumn(5, 1, 2, 3, 4, 5);
    BoolColumn *bc = new BoolColumn(2, true, false);
    FloatColumn *fc = new FloatColumn(4, 4.20, 4.20, 4.20, 4.20);
    StringColumn *sc = new StringColumn(2, new String("hello"), new String("world"));

    GT_TRUE(i->equals(ic->arr_));
    GT_TRUE(b->equals(bc->arr_));
    GT_TRUE(f->equals(fc->arr_));
    GT_TRUE(s->equals(sc->arr_));

    delete i;
    delete b;
    delete f;
    delete s;
    delete ic;
    delete bc;
    delete fc;
    delete sc;
    exit(0);
}

TEST(a4, p1) { ASSERT_EXIT_ZERO(doesVaListActuallyWork); }

/**
 * @brief Tests the get() function in all FastArrays
 * 
 */
void testGet()
{
    IntFastArray *i = new IntFastArray(5, 1, 2, 3, 4, 5);
    BoolFastArray *b = new BoolFastArray(2, true, false);
    FloatFastArray *f = new FloatFastArray(4, 4.20, 4.20, 4.20, 4.20);
    StringFastArray *s = new StringFastArray(2, new String("hello"), new String("world"));

    GT_EQUALS(i->get(0), 1);
    GT_EQUALS(i->get(4), 5);
    GT_EQUALS(b->get(0), true);
    GT_EQUALS(b->get(1), false);
    GT_TRUE(f->get(0) - 4.20 < 0.001 && f->get(0) - 4.20 > -.001);
    GT_TRUE(f->get(3) - 4.20 < 0.001 && f->get(3) - 4.20 > -.001);
    GT_TRUE(s->get(0)->equals(new String("hello")));
    GT_TRUE(s->get(1)->equals(new String("world")));

    exit(0);
}

TEST(a4, p2) { ASSERT_EXIT_ZERO(testGet); }

/**
 * @brief Tests the set() function in all FastArrays
 * 
 */
void testSet()
{
    IntFastArray *i = new IntFastArray(5, 1, 2, 3, 4, 5);
    BoolFastArray *b = new BoolFastArray(2, true, false);
    FloatFastArray *f = new FloatFastArray(4, 4.20, 4.20, 4.20, 4.20);
    StringFastArray *s = new StringFastArray(2, new String("hello"), new String("world"));

    i->set(0, 420);
    i->set(4, 69);
    GT_EQUALS(i->get(0), 420);
    GT_EQUALS(i->get(4), 69);

    b->set(0, true);
    b->set(1, false);
    GT_EQUALS(b->get(0), true);
    GT_EQUALS(b->get(1), false);

    f->set(0, 4.20);
    f->set(3, 6.9);
    GT_TRUE(f->get(0) - 4.20 < 0.001 && f->get(0) - 4.20 > -.001);
    GT_TRUE(f->get(3) - 6.9 < 0.001 && f->get(3) - 6.9 > -.001);

    String *h = new String("Heeellloooooo"); // no need to delete these as FastArrays own their contents
    String *w = new String("wooooorllddddd");
    s->set(0, h);
    s->set(1, w);
    GT_TRUE(s->get(0)->equals(h));
    GT_TRUE(s->get(1)->equals(w));

    exit(0);
}

TEST(a4, p3) { ASSERT_EXIT_ZERO(testSet); }

/**
 * @brief Tests the grow() function in all FastArrays by
 * adding 512 elements to each, which would grow
 * the outer array twice and the inner array(s) multiple
 * times
 * 
 * Also tests push_back() because we have to use that
 * function to test grow()
 * 
 */
void testGrowAndPushBack()
{
    IntFastArray *i = new IntFastArray(5, 1, 2, 3, 4, 5);
    BoolFastArray *b = new BoolFastArray(2, true, false);
    FloatFastArray *f = new FloatFastArray(4, 4.20, 4.20, 4.20, 4.20);
    StringFastArray *s = new StringFastArray(2, new String("hello"), new String("world"));

    for (int j = 0; j < 512; j++)
    {
        i->push_back(j);
        GT_EQUALS(i->get(j + 5), j);
    }

    for (int j = 0; j < 512; j++)
    {
        b->push_back(0);
        GT_EQUALS(b->get(j + 2), 0);
    }

    for (int j = 0; j < 512; j++)
    {
        f->push_back(4.20);
        GT_TRUE(f->get(j + 4) - 4.20 < 0.001 && f->get(j + 4) - 4.20 > -.001);
    }

    String *str = new String("hello world"); // FastArrays own their contents, so no need to delete this at the end
    for (int j = 0; j < 512; j++)
    {
        s->push_back(str);
        GT_TRUE(s->get(j + 2)->equals(str));
    }

    exit(0);
}

TEST(a4, p4) { ASSERT_EXIT_ZERO(testGrowAndPushBack); }

/********************************************************************************************************************
 * testing Schema
 * 
**/

/**
 * @brief Testing adding a Column with a name to a nonempty schema via checking that col_types is as expected and the 
 * number of columns is the expected number.
 * 
 */
void testAddColumn()
{
    String *s_str = new String("FIBS");
    Schema *s = new Schema(s_str->c_str());

    GT_TRUE(s->coltypes_->equals(s_str));
    s->add_column('S', new String("Lies"));
    String *s_str_new = new String("FIBSS");

    GT_EQUALS(s->width(), 5);
    GT_TRUE(s->coltypes_->equals(s_str_new));
    GT_TRUE(s->col_name(4)->equals(new String("Lies")));

    exit(0);
}

TEST(t6, addCol) { ASSERT_EXIT_ZERO(testAddColumn); }

/**
 * @brief Testing adding a Column without a name to a nonempty schema via checking that col_types is as expected and the 
 * number of columns is the expected number.
 * 
 */
void testAddEmptyNameColumn()
{
    String *s_str = new String("FIBS");
    Schema *s = new Schema(s_str->c_str());

    GT_TRUE(s->coltypes_->equals(s_str));
    s->add_column('S', nullptr);
    String *s_str_new = new String("FIBSS");

    GT_EQUALS(s->width(), 5);
    GT_TRUE(s->coltypes_->equals(s_str_new));
    GT_TRUE(s->col_name(4) == nullptr);

    exit(0);
}

TEST(t6, addNoName) { ASSERT_EXIT_ZERO(testAddEmptyNameColumn); }

/**
 * @brief Testing adding a Column without a name to a empty schema via checking that col_types is as expected and the 
 * number of columns is the expected number.
 * 
 */
void testAddColToEmptySchemaNoName()
{
    Schema *s = new Schema();

    s->add_column('S', nullptr);

    GT_EQUALS(s->width(), 1);
    GT_TRUE(s->coltypes_->equals(new String("S")));
    GT_TRUE(s->col_name(0) == nullptr);

    exit(0);
}

TEST(t6, addColEmptyNoName) { ASSERT_EXIT_ZERO(testAddColToEmptySchemaNoName); }

/**
 * @brief Testing adding a Column with a name to a empty schema via checking that col_types is as expected and the 
 * number of columns is the expected number.
 * 
 */
void testAddColToEmptySchema()
{
    Schema *s = new Schema();

    s->add_column('S', new String("Lies"));

    GT_EQUALS(s->width(), 1);
    GT_TRUE(s->coltypes_->equals(new String("S")));
    GT_TRUE(s->col_name(0)->equals(new String("Lies")));

    exit(0);
}

TEST(t6, addColEmpty) { ASSERT_EXIT_ZERO(testAddColToEmptySchema); }

/**
 * @brief Testing adding a row with a name to an empty schema via checking that the length of the schema has changed
 * and that the name of the row is as expected.
 * 
 */
void testAddRowToEmptySchema()
{
    Schema *s = new Schema();
    s->add_row(new String("easepease"));
    GT_TRUE(s->length() == 1);
    GT_TRUE(s->row_name(0)->equals(new String("easepease")));

    exit(0);
}

TEST(t6, addRowEmptySchema) { ASSERT_EXIT_ZERO(testAddRowToEmptySchema); }

/**
 * @brief Testing adding a row without a name to an empty schema via checking that the length of the schema has changed
 * and that the name of the row is as expected.
 * 
 */
void testAddNullRowToEmptySchema()
{
    Schema *s = new Schema();
    s->add_row(nullptr);
    GT_TRUE(s->row_name(0) == nullptr);

    exit(0);
}

TEST(t6, addNullRowEmptySchema) { ASSERT_EXIT_ZERO(testAddNullRowToEmptySchema); }

/**
 * @brief Testing adding a row with a name to an nonempty schema via checking that the length of the schema has changed
 * and that the name of the row is as expected.
 * 
 */
void testAddRowToSchema()
{
    String *s_str = new String("FIBS");
    Schema *s = new Schema(s_str->c_str());

    s->add_row(new String("easepease"));

    GT_TRUE(s->row_name(0)->equals(new String("easepease")));

    exit(0);
}

TEST(t6, addRowSchema) { ASSERT_EXIT_ZERO(testAddRowToSchema); }

/**
 * @brief Testing adding a row without a name to an nonempty schema via checking that the length of the schema has changed
 * and that the name of the row is as expected.
 * 
 */
void testAddNullRowToSchema()
{
    String *s_str = new String("FIBS");
    Schema *s = new Schema(s_str->c_str());

    s->add_row(nullptr);

    GT_TRUE(s->row_name(0) == nullptr);

    exit(0);
}

TEST(t6, addNullRowToSchema) { ASSERT_EXIT_ZERO(testAddNullRowToSchema); }

/**
 * @brief Testing getting column indexes on a schema that has no rows but has columns.
 * 
 */
void testGetColIdxEmptySchema()
{
    Schema *s = new Schema();

    s->add_column('B', new String("b"));
    s->add_column('I', new String("i"));
    s->add_column('F', new String("f"));
    s->add_column('S', new String("s"));

    GT_EQUALS(s->width(), 4);
    GT_EQUALS(s->col_idx("b"), 0);
    GT_EQUALS(s->col_idx("i"), 1);
    GT_EQUALS(s->col_idx("f"), 2);
    GT_EQUALS(s->col_idx("s"), 3);
    GT_EQUALS(s->col_idx("not existent"), -1);

    exit(0);
}

TEST(t6, getColIdxEmpty) { ASSERT_EXIT_ZERO(testGetColIdxEmptySchema); }

/**
 * @brief esting getting column indexes on a schema that has no rows but has columns after adding more columns.
 * 
 */
void testGetColIdx()
{
    String *s_str = new String("FIBS");
    Schema *s = new Schema(s_str->c_str());

    s->add_column('B', new String("b"));
    s->add_column('I', new String("i"));
    s->add_column('F', new String("f"));
    s->add_column('S', new String("s"));

    GT_EQUALS(s->width(), 8);
    GT_EQUALS(s->col_idx("b"), 4);
    GT_EQUALS(s->col_idx("i"), 5);
    GT_EQUALS(s->col_idx("f"), 6);
    GT_EQUALS(s->col_idx("s"), 7);

    exit(0);
}

TEST(t6, getColIdx) { ASSERT_EXIT_ZERO(testGetColIdx); }

/**
 * @brief Testing getting row indexes on a schema with no columns.
 * 
 */
void testGetRowIdxEmptySchema()
{
    Schema *s = new Schema();

    s->add_row(new String("b"));
    s->add_row(new String("i"));
    s->add_row(new String("f"));
    s->add_row(new String("s"));

    GT_EQUALS(s->length(), 4);
    GT_EQUALS(s->row_idx("b"), 0);
    GT_EQUALS(s->row_idx("i"), 1);
    GT_EQUALS(s->row_idx("f"), 2);
    GT_EQUALS(s->row_idx("s"), 3);
    GT_EQUALS(s->row_idx("no"), -1);

    exit(0);
}

TEST(t6, getRowIdxEmpty) { ASSERT_EXIT_ZERO(testGetRowIdxEmptySchema); }

/**
 * @brief Testing getting row indexes on a schema with columns.
 * 
 */
void testGetRowIdx()
{
    String *s_str = new String("FIBS");
    Schema *s = new Schema(s_str->c_str());

    s->add_row(new String("b"));
    s->add_row(new String("i"));
    s->add_row(new String("f"));
    s->add_row(new String("s"));

    GT_EQUALS(s->length(), 4);
    GT_EQUALS(s->row_idx("b"), 0);
    GT_EQUALS(s->row_idx("i"), 1);
    GT_EQUALS(s->row_idx("f"), 2);
    GT_EQUALS(s->row_idx("s"), 3);
    GT_EQUALS(s->row_idx("blah"), -1);

    exit(0);
}

TEST(t6, getRowIdx) { ASSERT_EXIT_ZERO(testGetRowIdx); }

/********************************************************************************************************************
 * testing an implementation Rower iterating over Rows
 * 
 **/

/**
 * @brief tests Rower's accept() function on rows populated via a nonempty dataframe/schema.
 * 
 */
void testAcceptNonemptyRow()
{
    Schema *s = new Schema("IISFB");    // nonempty schema
    DataFrame *df = new DataFrame(*s);  // make df based on nonempty schema
    Row *r = new Row(df->get_schema()); // making a row based off the schema
    GT_TRUE(true);
    // filling df with rows
    for (size_t i = 0; i < 1000; i++)
    {
        r->set(0, (int)i);
        r->set(1, (int)i + 1);
        r->set(2, new String("string hello"));
        r->set(3, (float)(i + 0.22));
        r->set(4, i % 2 == 1);
        df->add_row(*r);
    }
    delete r;
    RowPrinter printer_ = RowPrinter(); // creating rower object
    // iterating through the rows and checking that the rower accepts each row given to it
    Row *r_ = new Row(df->get_schema());
    for (size_t i = 0; i < df->nrows(); i++)
    {
        Row *r_ = new Row(df->get_schema());
        df->fill_row(i, *r_);
        GT_TRUE(printer_.accept(*r_));
        delete r_;
    }
    exit(0);
}

TEST(t1, testAcceptNonemptyRow)
{
    ASSERT_EXIT_ZERO(testAcceptNonemptyRow);
}

/**
 * @brief tests Rower's accept() function on a row created via an empty Schema
 * 
 */
void testAcceptEmptyRow()
{
    Schema *s = new Schema(); // empty schema
    Row r = Row(*s);          // making a row from empty schema
    RowPrinter rower_ = RowPrinter();
    GT_TRUE(rower_.accept(r));
    delete s;
    exit(0);
}

TEST(t1, testAcceptEmptyRow)
{
    ASSERT_EXIT_ZERO(testAcceptEmptyRow);
}

/**
 * @brief tests Rower's accept() function on a row created from an dataframe based on Schema that has no columns but many rows
 * 
 */
void testAcceptEmptyRow2()
{
    Schema *s = new Schema(); // empty schema
    // adding rows to schema
    for (size_t i = 0; i < 1000; i++)
    {
        s->add_row(nullptr);
    }
    DataFrame *df = new DataFrame(*s); // dataframe which has 1000 rows and no columns
    RowPrinter rower_ = RowPrinter();  // creating rower object
    // iterating through the rows and checking that the rower accepts each row given to it
    Row *r_ = new Row(df->get_schema());
    for (size_t i = 0; i < df->nrows(); i++)
    {
        r_ = new Row(df->get_schema());
        df->fill_row(i, *r_);
        GT_TRUE(rower_.accept(*r_));
        //delete r_;
    }
    exit(0);
}

TEST(t1, testAcceptEmptyRow2)
{
    ASSERT_EXIT_ZERO(testAcceptEmptyRow2);
}

/********************************************************************************************************************
 * testing Columns
 * 
 **/

/**
 * @brief Checking to see if we can accurately get the elements from an IntColumn
 * 
 */
void testIntColumnGet1()
{
    IntColumn *i = new IntColumn(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    for (int j = 0; j < 17; j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == j);
    }
    delete i;
    exit(0);
}

TEST(t2, int1)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnGet1);
}

/**
 * @brief Testing set function via setting the values then getting them to check if we set the value correctly as each location.
 * 
 */
void testIntColumnSet2()
{
    IntColumn *i = new IntColumn(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    // set 1 value in i to something else and check if that one value changed correctly
    i->set(8, 8888);
    CS4500_ASSERT_TRUE(i->get(8) == 8888);
    // set all values in i to something else and check
    for (int j = 0; j < 17; j++)
    {
        i->set(j, j * 3);
    }
    for (size_t j = 0; j < 17; j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == j * 3);
    }
    delete i;
    exit(0);
}

TEST(t2, int2)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnSet2);
}

/**
 * @brief Testing that push_back() function works as expected for empty and nonempty Columns. Check after each push back that the added element is
 * where and what is expected.
 * 
 */
void testIntColumnPushBack3()
{
    // checking empty column
    IntColumn *i_ = new IntColumn();
    // adding to empty column and checking to see if added element is where and what we expect
    for (size_t j = 0; j < 10000000; j++)
    {
        i_->push_back(j);
        CS4500_ASSERT_TRUE(i_->get(j) == j);
    }
    delete i_;

    // checking nonempty array
    IntColumn *i = new IntColumn(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    for (size_t j = 0; j < 10000000; j++)
    {
        i->push_back(j * 3);
        CS4500_ASSERT_TRUE(i->get(j + 17) == (j * 3));
    }
    delete i;
    exit(0);
}

TEST(t2, int3)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnPushBack3);
}

/**
 * @brief Testing that size works as expected for empty and nonempty Columns including after we add new elements via push_back() function
 * 
 */
void testIntColumnSize4()
{
    // checking empty column
    IntColumn *i_ = new IntColumn();
    CS4500_ASSERT_TRUE(i_->size() == 0);
    // adding to empty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i_->push_back(j);
        CS4500_ASSERT_TRUE(i_->size() == (j + 1));
    }
    CS4500_ASSERT_TRUE(i_->size() == 10000000);
    delete i_;
    // checking nonempty column
    IntColumn *i = new IntColumn(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    CS4500_ASSERT_TRUE(i->size() == 17);
    // adding to nonempty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i->push_back(j);
        CS4500_ASSERT_TRUE(i->size() == (j + 18));
    }
    for (size_t j = 0; j < 17; j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == j);
    }
    delete i;
    exit(0);
}

TEST(t2, int4)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnSize4);
}

/**
 * @brief Testing if empty/nonempty IntColumn returns the expected pointer
 * 
 */
void testIntColumnAsType5()
{
    // checking empty column
    IntColumn *i_ = new IntColumn();
    CS4500_ASSERT_TRUE(i_ == i_->as_int());
    CS4500_ASSERT_TRUE(i_->as_bool() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_float() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_string() == nullptr);
    // checking nonempty column
    IntColumn *i = new IntColumn(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    CS4500_ASSERT_TRUE(i == i->as_int());
    CS4500_ASSERT_TRUE(i->as_bool() == nullptr);
    CS4500_ASSERT_TRUE(i->as_float() == nullptr);
    CS4500_ASSERT_TRUE(i->as_string() == nullptr);
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, int5)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnAsType5);
}

/**
 * @brief Testing if empty/nonempty IntColumn returns the expected char representing its type
 * 
 */
void testIntColumnGetType6()
{
    // checking empty column
    Column *i_ = new IntColumn();
    CS4500_ASSERT_TRUE(i_->get_type() == 'I');
    // checking nonempty column
    IntColumn *i = new IntColumn(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    CS4500_ASSERT_TRUE(i->get_type() == 'I');
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, int6)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnGetType6);
}

/**
 * @brief Tests the clone method for an IntColumn. Because Columns have pointer equality, to see if clone is correct, we test to see if each element is the same. 
 * Additionally, because a clone should not directly point at something the Column owns, the original Column is deleted and then the values in the clone are checked
 * again.
 * 
 */
void testIntColumnClone7()
{
    // create and populate IntCol
    IntColumn *i = new IntColumn();
    for (size_t j = 0; j < 10000; j++)
    {
        i->push_back(j);
    }
    IntColumn *i_ = i->clone();
    CS4500_ASSERT_TRUE(i->size() == i_->size());
    // check manually if each element is the same
    for (size_t j = 0; j < i->size(); j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == i_->get(j));
    }
    delete i;
    // check that the clone retains information/elements after original is deleted
    for (size_t j = 0; j < 10000; j++)
    {
        CS4500_ASSERT_TRUE(i_->get(j) == j);
    }
    delete i_;
    exit(0);
}

TEST(t2, int7)
{
    CS4500_ASSERT_EXIT_ZERO(testIntColumnClone7);
}

/**
 * @brief Checking to see if we can accurately get the elements from an BoolColumn
 * 
 */
void testBoolColumnGet1()
{
    BoolColumn *i = new BoolColumn(18, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
    for (size_t j = 0; j < 18; j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == (j % 2 == 0));
    }
    delete i;
    exit(0);
}

TEST(t2, Bool1)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnGet1);
}

/**
 * @brief Testing set function via setting the values then getting them to check if we set the value correctly as each location.
 * 
 */
void testBoolColumnSet2()
{
    BoolColumn *i = new BoolColumn(18, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
    // set 1 value in i to something else and check if that one value changed correctly
    i->set(8, false);
    CS4500_ASSERT_FALSE(i->get(8));
    // set all values in i to something else and check
    for (size_t j = 0; j < 18; j++)
    {
        i->set(j, (j % 2 == 1));
    }
    for (size_t j = 0; j < 18; j++)
    {
        bool expected = (j % 2 == 1);
        CS4500_ASSERT_TRUE(i->get(j) == expected);
    }
    delete i;
    exit(0);
}

TEST(t2, Bool2)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnSet2);
}

/**
 * @brief Testing that push_back() function works as expected for empty and nonempty Columns. Check after each push back that the added element is
 * where and what is expected.
 * 
 */
void testBoolColumnPushBack3()
{
    // checking empty column
    BoolColumn *i_ = new BoolColumn();
    // adding to empty column and checking to see if added element is where and what we expect
    for (size_t j = 0; j < 10000000; j++)
    {
        i_->push_back(j % 2 == 0);
        CS4500_ASSERT_TRUE(i_->get(j) == (j % 2 == 0));
    }
    delete i_;

    // checking nonempty array
    BoolColumn *i = new BoolColumn(18, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
    for (size_t j = 0; j < 10000000; j++)
    {
        i->push_back((j % 2 == 1));
        CS4500_ASSERT_TRUE(i->get(j + 18) == (j % 2 == 1));
    }
    // check that the beginning is unchanged by pushback
    for (size_t j = 0; j < 18; j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == (j % 2 == 0));
    }
    delete i;
    exit(0);
}

TEST(t2, Bool3)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnPushBack3);
}

/**
 * @brief Testing that size works as expected for empty and nonempty Columns including after we add new elements via push_back() function
 * 
 */
void testBoolColumnSize4()
{
    // checking empty column
    BoolColumn *i_ = new BoolColumn();
    CS4500_ASSERT_TRUE(i_->size() == 0);
    // adding to empty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i_->push_back(true);
        CS4500_ASSERT_TRUE(i_->size() == (j + 1));
    }
    CS4500_ASSERT_TRUE(i_->size() == 10000000);
    delete i_;
    // checking nonempty column
    BoolColumn *i = new BoolColumn(18, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
    CS4500_ASSERT_TRUE(i->size() == 18);
    // adding to nonempty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i->push_back(true);
        CS4500_ASSERT_TRUE(i->size() == (j + 19));
    }
    //CS4500_ASSERT_TRUE(i->size() == 10000018);
    delete i;
    exit(0);
}

TEST(t2, Bool4)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnSize4);
}

/**
 * @brief Testing if empty/nonempty BoolColumn returns the expected poBooler
 * 
 */
void testBoolColumnAsType5()
{
    // checking empty column
    BoolColumn *i_ = new BoolColumn();
    CS4500_ASSERT_TRUE(i_ == i_->as_bool());
    CS4500_ASSERT_TRUE(i_->as_int() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_float() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_string() == nullptr);
    // checking nonempty column
    BoolColumn *i = new BoolColumn(18, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
    CS4500_ASSERT_TRUE(i == i->as_bool());
    CS4500_ASSERT_TRUE(i->as_int() == nullptr);
    CS4500_ASSERT_TRUE(i->as_float() == nullptr);
    CS4500_ASSERT_TRUE(i->as_string() == nullptr);
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, Bool5)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnAsType5);
}

/**
 * @brief Testing if empty/nonempty BoolColumn returns the expected char representing its type
 * 
 */
void testBoolColumnGetType6()
{
    // checking empty column
    Column *i_ = new BoolColumn();
    CS4500_ASSERT_TRUE(i_->get_type() == 'B');
    // checking nonempty column
    BoolColumn *i = new BoolColumn(18, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false);
    CS4500_ASSERT_TRUE(i->get_type() == 'B');
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, Bool6)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnGetType6);
}

/**
 * @brief Tests the clone method for an BoolColumn. Because Columns have pointer equality, to see if clone is correct, we test to see if each element is the same. 
 * Additionally, because a clone should not directly pointer at something the Column owns, the original Column is deleted and then the values in the clone are checked
 * again.
 * 
 */
void testBoolColumnClone7()
{
    // create and populate BoolCol
    BoolColumn *i = new BoolColumn();
    for (size_t j = 0; j < 10000; j++)
    {
        i->push_back((j % 2 == 1));
    }
    BoolColumn *i_ = i->clone();
    CS4500_ASSERT_TRUE(i->size() == i_->size());
    // check manually if each element is the same
    for (size_t j = 0; j < i->size(); j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) == i_->get(j));
    }
    delete i;
    // check that the clone retains information/elements after original is deleted
    for (size_t j = 0; j < 10000; j++)
    {
        CS4500_ASSERT_TRUE(i_->get(j) == (j % 2 == 1));
    }
    delete i_;
    exit(0);
}

TEST(t2, Bool7)
{
    CS4500_ASSERT_EXIT_ZERO(testBoolColumnClone7);
}

/**
 * @brief Checking to see if we can accurately get the elements from an FloatColumn
 * 
 */
void testFloatColumnGet1()
{
    FloatColumn *i = new FloatColumn(17, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22, 9.22, 10.22, 11.22, 12.22, 13.22, 14.22, 15.22, 16.22, 17.22);
    for (size_t j = 0; j < 17; j++)
    {
        float expected = float(j) + 1.22;
        CS4500_ASSERT_TRUE(i->get(j) - expected < 0.001);
        //CS4500_ASSERT_TRUE(i->get(j) - expected > -0.001);
    }
    delete i;
    exit(0);
}

TEST(t2, Float1)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnGet1);
}

/**
 * @brief Testing set function via setting the values then getting them to check if we set the value correctly as each location.
 * 
 */
void testFloatColumnSet2()
{
    FloatColumn *i = new FloatColumn(17, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22, 9.22, 10.22, 11.22, 12.22, 13.22, 14.22, 15.22, 16.22, 17.22);
    // set 1 value in i to something else and check if that one value changed correctly
    i->set(8, 8.88);
    CS4500_ASSERT_TRUE(i->get(8) - 8.88 < 0.001 && i->get(8) - 8.88 > -0.001);
    // set all values in i to something else and check
    for (size_t j = 0; j < 17; j++)
    {
        i->set(j, (float)j + 2.22);
    }
    for (size_t j = 0; j < 17; j++)
    {
        float expected = float(j) + 2.22;
        CS4500_ASSERT_TRUE((i->get(j) - expected) < 0.001);
        CS4500_ASSERT_TRUE((i->get(j) - expected) > -0.001);
    }
    delete i;
    exit(0);
}

TEST(t2, Float2)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnSet2);
}

/**
 * @brief Testing that push_back() function works as expected for empty and nonempty Columns. Check after each push back that the added element is
 * where and what is expected.
 * 
 */
void testFloatColumnPushBack3()
{
    // checking empty column
    FloatColumn *i_ = new FloatColumn();
    // adding to empty column and checking to see if added element is where and what we expect
    for (size_t j = 0; j < 10000000; j++)
    {
        float expected = float(j) + 2.42;
        i_->push_back(expected);
        CS4500_ASSERT_TRUE((i_->get(j) - expected) < 0.001);
        CS4500_ASSERT_TRUE((i_->get(j) - expected) > -0.001);
    }
    delete i_;

    // checking nonempty array
    FloatColumn *i = new FloatColumn(17, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22, 9.22, 10.22, 11.22, 12.22, 13.22, 14.22, 15.22, 16.22, 17.22);
    for (size_t j = 0; j < 10000000; j++)
    {
        float expected = float(j) + 2.44;
        i->push_back(expected);
        CS4500_ASSERT_TRUE((i->get(j + 17) - expected) < 0.001);
        CS4500_ASSERT_TRUE((i->get(j + 17) - expected) > -0.001);
    }
    // check that the beginning is unchanged by pushback
    for (size_t j = 0; j < 17; j++)
    {
        float expected = float(j) + 1.22;
        CS4500_ASSERT_TRUE((i->get(j) - expected) < 0.001);
        CS4500_ASSERT_TRUE((i->get(j) - expected) > -0.001);
    }
    delete i;

    exit(0);
}

TEST(t2, Float3)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnPushBack3);
}

/**
 * @brief Testing that size works as expected for empty and nonempty Columns including after we add new elements via push_back() function
 * 
 */
void testFloatColumnSize4()
{
    // checking empty column
    FloatColumn *i_ = new FloatColumn();
    CS4500_ASSERT_TRUE(i_->size() == 0);
    // adding to empty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i_->push_back(2.22);
        CS4500_ASSERT_TRUE(i_->size() == (j + 1));
    }
    CS4500_ASSERT_TRUE(i_->size() == 10000000);
    delete i_;
    // checking nonempty column
    FloatColumn *i = new FloatColumn(17, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22, 9.22, 10.22, 11.22, 12.22, 13.22, 14.22, 15.22, 16.22, 17.22);
    CS4500_ASSERT_TRUE(i->size() == 17);
    // adding to nonempty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i->push_back(2.22);
        CS4500_ASSERT_TRUE(i->size() == (j + 18));
    }
    //CS4500_ASSERT_TRUE(i->size() == 10000017);
    delete i;
    exit(0);
}

TEST(t2, Float4)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnSize4);
}

/**
 * @brief Testing if empty/nonempty FloatColumn returns the expected poFloater
 * 
 */
void testFloatColumnAsType5()
{
    // checking empty column
    FloatColumn *i_ = new FloatColumn();
    CS4500_ASSERT_TRUE(i_ == i_->as_float());
    CS4500_ASSERT_TRUE(i_->as_bool() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_int() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_string() == nullptr);
    // checking nonempty column
    FloatColumn *i = new FloatColumn(17, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22, 9.22, 10.22, 11.22, 12.22, 13.22, 14.22, 15.22, 16.22, 17.22);
    CS4500_ASSERT_TRUE(i == i->as_float());
    CS4500_ASSERT_TRUE(i->as_bool() == nullptr);
    CS4500_ASSERT_TRUE(i->as_int() == nullptr);
    CS4500_ASSERT_TRUE(i->as_string() == nullptr);
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, Float5)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnAsType5);
}

/**
 * @brief Testing if empty/nonempty FloatColumn returns the expected char representing its type
 * 
 */
void testFloatColumnGetType6()
{
    // checking empty column
    Column *i_ = new FloatColumn();
    CS4500_ASSERT_TRUE(i_->get_type() == 'F');
    // checking nonempty column
    FloatColumn *i = new FloatColumn(17, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22, 9.22, 10.22, 11.22, 12.22, 13.22, 14.22, 15.22, 16.22, 17.22);
    CS4500_ASSERT_TRUE(i->get_type() == 'F');
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, Float6)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnGetType6);
}

/**
 * @brief Tests the clone method for an FloatColumn. Because Columns have pointer equality, to see if clone is correct, we test to see if each element is the same. 
 * Additionally, because a clone should not directly pointer at something the Column owns, the original Column is deleted and then the values in the clone are checked
 * again.
 * 
 */
void testFloatColumnClone7()
{
    // create and populate FloatCol
    FloatColumn *i = new FloatColumn();
    for (size_t j = 0; j < 10000; j++)
    {
        i->push_back(j + 2.345);
    }
    FloatColumn *i_ = i->clone();
    CS4500_ASSERT_TRUE(i->size() == i_->size());
    // check manually if each element is the same
    for (size_t j = 0; j < i->size(); j++)
    {
        CS4500_ASSERT_TRUE(i->get(j) - i_->get(j) < 0.001);
        CS4500_ASSERT_TRUE(i->get(j) - i_->get(j) > -0.001);
    }
    delete i;
    // check that the clone retains information/elements after original is deleted
    for (size_t j = 0; j < 10000; j++)
    {
        float expected = j + 2.345;
        CS4500_ASSERT_TRUE(i_->get(j) - expected < 0.001);
        CS4500_ASSERT_TRUE(i_->get(j) - expected > -0.001);
    }
    delete i_;
    exit(0);
}

TEST(t2, Float7)
{
    CS4500_ASSERT_EXIT_ZERO(testFloatColumnClone7);
}

/**
 * @brief Checking to see if we can accurately get the elements from an StringColumn
 * 
 */
void testStringColumnGet1()
{
    String *s1 = new String("h");
    String *s2 = new String("e");
    String *s3 = new String("l");
    String *s4 = new String("l");
    String *s5 = new String("o");
    String *s6 = new String("w");
    String *s7 = new String("o");
    String *s8 = new String("r");
    String *s9 = new String("l");
    String *s10 = new String("d");
    String *s11 = new String("r");
    String *s12 = new String("e");
    String *s13 = new String("j");
    String *s14 = new String("e");
    String *s15 = new String("c");
    String *s16 = new String("t");
    String *s17 = new String("s");
    String *all = new String("helloworldrejects");
    StringColumn *i = new StringColumn(17, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);
    char *c;
    String *expected_;
    for (size_t j = 0; j < 17; j++)
    {
        c = new char(all->at(j));
        expected_ = new String(c);
        CS4500_ASSERT_TRUE(i->get(j)->equals(expected_));
        delete expected_;
        delete c;
    }
    delete i;
    delete all;
    exit(0);
}

TEST(t2, String1)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnGet1);
}

/**
 * @brief Testing set function via setting the values then getting them to check if we set the value correctly as each location.
 * 
 */
void testStringColumnSet2()
{
    String *s1 = new String("h");
    String *s2 = new String("e");
    String *s3 = new String("l");
    String *s4 = new String("l");
    String *s5 = new String("o");
    String *s6 = new String("w");
    String *s7 = new String("o");
    String *s8 = new String("r");
    String *s9 = new String("l");
    String *s10 = new String("d");
    String *s11 = new String("r");
    String *s12 = new String("e");
    String *s13 = new String("j");
    String *s14 = new String("e");
    String *s15 = new String("c");
    String *s16 = new String("t");
    String *s17 = new String("s");
    String *all = new String("helloworldrejects");
    String *new_ = new String("blah");
    String *all2 = new String("blahhityblahhityblah");
    StringColumn *i = new StringColumn(17, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);
    // set 1 value in i to something else and check if that one value changed correctly
    i->set(8, new String("blah"));
    CS4500_ASSERT_TRUE(i->get(8)->equals(new_));
    // set all values in i to something else and check
    char *c;
    String *expected_;
    for (size_t j = 0; j < 17; j++)
    {
        c = new char(all2->at(j));
        expected_ = new String(c);
        i->set(j, expected_->clone());
        delete c;
        delete expected_;
    }
    for (size_t j = 0; j < 17; j++)
    {
        c = new char(all2->at(j));
        expected_ = new String(c);
        CS4500_ASSERT_TRUE(i->get(j)->equals(expected_));
        delete c;
        delete expected_;
    }
    delete i;
    delete all;
    delete all2;
    delete new_;
    exit(0);
}

TEST(t2, String2)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnSet2);
}

/**
 * @brief Testing that push_back() function works as expected for empty and nonempty Columns. Check after each push back that the added element is
 * where and what is expected.
 * 
 */
void testStringColumnPushBack3()
{
    String *all = new String("blahhityblahhityblah");
    size_t size_ = all->size();
    // checking empty column
    StringColumn *i_ = new StringColumn();
    char *c;
    String *expected;
    // adding to empty column and checking to see if added element is where and what we expect
    for (size_t j = 0; j < 10000000; j++)
    {
        c = new char(all->at(j % size_));
        expected = new String(c);
        i_->push_back(expected->clone());
        CS4500_ASSERT_TRUE(i_->get(j)->equals(expected));
        delete c;
        delete expected;
    }
    delete i_;
    // checking nonempty array
    String *s1 = new String("h");
    String *s2 = new String("e");
    String *s3 = new String("l");
    String *s4 = new String("l");
    String *s5 = new String("o");
    String *s6 = new String("w");
    String *s7 = new String("o");
    String *s8 = new String("r");
    String *s9 = new String("l");
    String *s10 = new String("d");
    String *s11 = new String("r");
    String *s12 = new String("e");
    String *s13 = new String("j");
    String *s14 = new String("e");
    String *s15 = new String("c");
    String *s16 = new String("t");
    String *s17 = new String("s");
    String *all2 = new String("helloworldrejects");
    StringColumn *i = new StringColumn(17, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);
    for (size_t j = 0; j < 10000; j++)
    {
        char *c = new char(all->at(j % size_));
        String *expected = new String(c);
        i->push_back(expected->clone());
        CS4500_ASSERT_TRUE(i->size() == j + 18);
        CS4500_ASSERT_TRUE(i->get(j + 17)->equals(expected));
    }
    // check that the beginning is unchanged by pushback
    for (size_t j = 0; j < 17; j++)
    {
        c = new char(all2->at(j));
        expected = new String(c);
        CS4500_ASSERT_TRUE(i->get(j)->equals(expected));
        delete c;
        delete expected;
    }
    delete i;
    delete all;
    delete all2;
    exit(0);
}

TEST(t2, String3)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnPushBack3);
}

/**
 * @brief Testing that size works as expected for empty and nonempty Columns including after we add new elements via push_back() function
 * 
 */
void testStringColumnSize4()
{
    // checking empty column
    StringColumn *i_ = new StringColumn();
    CS4500_ASSERT_TRUE(i_->size() == 0);
    // adding to empty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i_->push_back(new String("h"));
        CS4500_ASSERT_TRUE(i_->size() == (j + 1));
    }
    CS4500_ASSERT_TRUE(i_->size() == 10000000);
    delete i_;
    // checking nonempty column
    String *s1 = new String("h");
    String *s2 = new String("e");
    String *s3 = new String("l");
    String *s4 = new String("l");
    String *s5 = new String("o");
    String *s6 = new String("w");
    String *s7 = new String("o");
    String *s8 = new String("r");
    String *s9 = new String("l");
    String *s10 = new String("d");
    String *s11 = new String("r");
    String *s12 = new String("e");
    String *s13 = new String("j");
    String *s14 = new String("e");
    String *s15 = new String("c");
    String *s16 = new String("t");
    String *s17 = new String("s");
    StringColumn *i = new StringColumn(17, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);
    CS4500_ASSERT_TRUE(i->size() == 17);
    // adding to nonempty column and checking to see if size is as expected after each add and at the end
    for (size_t j = 0; j < 10000000; j++)
    {
        i->push_back(new String("h"));
        CS4500_ASSERT_TRUE(i->size() == (j + 18));
    }
    CS4500_ASSERT_TRUE(i->size() == 10000017);
    delete i;
    exit(0);
}

TEST(t2, String4)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnSize4);
}

/**
 * @brief Testing if empty/nonempty StringColumn returns the expected pointer
 * 
 */
void testStringColumnAsType5()
{
    // checking empty column
    StringColumn *i_ = new StringColumn();
    CS4500_ASSERT_TRUE(i_ == i_->as_string());
    CS4500_ASSERT_TRUE(i_->as_bool() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_int() == nullptr);
    CS4500_ASSERT_TRUE(i_->as_float() == nullptr);
    // checking nonempty column
    String *s1 = new String("h");
    String *s2 = new String("e");
    String *s3 = new String("l");
    String *s4 = new String("l");
    String *s5 = new String("o");
    String *s6 = new String("w");
    String *s7 = new String("o");
    String *s8 = new String("r");
    String *s9 = new String("l");
    String *s10 = new String("d");
    String *s11 = new String("r");
    String *s12 = new String("e");
    String *s13 = new String("j");
    String *s14 = new String("e");
    String *s15 = new String("c");
    String *s16 = new String("t");
    String *s17 = new String("s");
    StringColumn *i = new StringColumn(17, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);
    CS4500_ASSERT_TRUE(i == i->as_string());
    CS4500_ASSERT_TRUE(i->as_bool() == nullptr);
    CS4500_ASSERT_TRUE(i->as_int() == nullptr);
    CS4500_ASSERT_TRUE(i->as_float() == nullptr);
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, String5)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnAsType5);
}

/**
 * @brief Testing if empty/nonempty StringColumn returns the expected char representing its type
 * 
 */
void testStringColumnGetType6()
{
    // checking empty column
    Column *i_ = new StringColumn();
    CS4500_ASSERT_TRUE(i_->get_type() == 'S');
    // checking nonempty column
    String *s1 = new String("h");
    String *s2 = new String("e");
    String *s3 = new String("l");
    String *s4 = new String("l");
    String *s5 = new String("o");
    String *s6 = new String("w");
    String *s7 = new String("o");
    String *s8 = new String("r");
    String *s9 = new String("l");
    String *s10 = new String("d");
    String *s11 = new String("r");
    String *s12 = new String("e");
    String *s13 = new String("j");
    String *s14 = new String("e");
    String *s15 = new String("c");
    String *s16 = new String("t");
    String *s17 = new String("s");
    StringColumn *i = new StringColumn(17, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17);
    CS4500_ASSERT_TRUE(i->get_type() == 'S');
    delete i;
    delete i_;
    exit(0);
}

TEST(t2, String6)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnGetType6);
}

/**
 * @brief Tests the clone method for an StringColumn. Because Columns have pointer equality, to see if clone is correct, we test to see if each element is the same. 
 * Additionally, because a clone should not directly pointer at something the Column owns, the original Column is deleted and then the values in the clone are checked
 * again.
 * 
 */
void testStringColumnClone7()
{
    // create and populate StringCol
    String *all = new String("blahhityblahhityblah");
    size_t size_ = all->size();
    StringColumn *i = new StringColumn();
    char *c;
    String *expected;
    for (size_t j = 0; j < 10000; j++)
    {
        c = new char(all->at(j % size_));
        expected = new String(c);
        i->push_back(expected->clone());
    }
    StringColumn *i_ = i->clone();
    CS4500_ASSERT_TRUE(i->size() == i_->size());
    // check manually if each element is the same
    for (size_t j = 0; j < i->size(); j++)
    {
        CS4500_ASSERT_TRUE(i->get(j)->equals(i_->get(j)));
    }
    delete i;
    // check that the clone retains information/elements after original is deleted
    for (size_t j = 0; j < 10000; j++)
    {
        c = new char(all->at(j % size_));
        expected = new String(c);
        CS4500_ASSERT_TRUE(i_->get(j)->equals(expected));
    }
    delete c;
    delete expected;
    exit(0);
}

TEST(t2, String7)
{
    CS4500_ASSERT_EXIT_ZERO(testStringColumnClone7);
}

/*****************************************************************************************************************
 * testing dataframes
 **/

/**
 * @brief Getting the schema from a Dataframe. Testing if it has the expected row/columns as well as the 
 * elements.
 * 
 */
void testGetSchema1()
{
    // empty schema, no cols/rows
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);
    Schema got = df->get_schema();
    CS4500_ASSERT_TRUE(got.length() == s->length());
    CS4500_ASSERT_TRUE(got.width() == s->width());

    // nonempty schema, with only cols
    Schema *s2 = new Schema("IFIBS");
    DataFrame *df2 = new DataFrame(*s2);
    Schema got2 = df2->get_schema();
    CS4500_ASSERT_TRUE(got2.length() == s2->length());
    CS4500_ASSERT_TRUE(got2.width() == s2->width());
    for (size_t i = 0; i < got2.width(); i++)
    {
        CS4500_ASSERT_TRUE(got2.col_type(i) == s2->col_type(i));
    }
    exit(0);
}

TEST(dataframe, test1) { CS4500_ASSERT_EXIT_ZERO(testGetSchema1); }

/**
 * @brief Testing adding a column and then testing the dataframe's schema has changed as expected. 
 * Due to how this test is structured, we also can also use this test to test nrows() and ncols() functions. 
 * 
 */
void testAddColumn2()
{
    // empty schema, no cols/rows
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);
    CS4500_ASSERT_TRUE(df->nrows() == 0);
    CS4500_ASSERT_TRUE(df->ncols() == 0);
    Column *c = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
    df->add_column(c, new String("name"));
    CS4500_ASSERT_TRUE(df->get_schema().width() == 1);
    CS4500_ASSERT_TRUE(df->get_schema().length() == 8);
    CS4500_ASSERT_TRUE(df->get_schema().col_name(0)->equals(new String("name")));
    CS4500_ASSERT_TRUE(df->get_schema().col_type(0) == 'I');
    CS4500_ASSERT_TRUE(df->nrows() == 8);
    CS4500_ASSERT_TRUE(df->ncols() == 1);
    // now adding another column to nonempty dfs
    Column *cc = new BoolColumn(8, true, false, true, false, true, false, true, false);
    df->add_column(cc, new String("2nd col"));
    CS4500_ASSERT_TRUE(df->get_schema().width() == 2);
    CS4500_ASSERT_TRUE(df->get_schema().length() == 8);
    CS4500_ASSERT_TRUE(df->get_schema().col_name(1)->equals(new String("2nd col")));
    CS4500_ASSERT_TRUE(df->get_schema().col_type(1) == 'B');
    CS4500_ASSERT_TRUE(df->nrows() == 8);
    CS4500_ASSERT_TRUE(df->ncols() == 2);
    exit(0);
}

TEST(dataframe, test2) { CS4500_ASSERT_EXIT_ZERO(testAddColumn2); }

/**
 * @brief Testing adding row by seeing if the dataframe's schema and other values have changed as expected. 
 * 
 */
void testAddRow3()
{
    // adding row to a nonempty schema
    Schema *s = new Schema("IISFB");    // nonempty schema
    DataFrame *df = new DataFrame(*s);  // make df based on nonempty schema
    Row *r = new Row(df->get_schema()); // making a row based off the schema
    // filling df with rows
    for (size_t i = 0; i < 1000; i++)
    {
        r->set(0, (int)i);
        r->set(1, (int)i + 1);
        r->set(2, new String("string hello"));
        r->set(3, (float)(i + 0.22));
        r->set(4, i % 2 == 1);
        df->add_row(*r);
    }
    CS4500_ASSERT_TRUE(df->get_schema().length() == 1000);
    CS4500_ASSERT_TRUE(df->get_schema().width() == 5);
    CS4500_ASSERT_TRUE(df->nrows() == 1000);
    CS4500_ASSERT_TRUE(df->ncols() == 5);
    exit(0);
}

TEST(dataframe, test3) { CS4500_ASSERT_EXIT_ZERO(testAddRow3); }

/**
 * @brief Checking that in a nonempty 
 * 
 */
void testGet4()
{
    // empty schema, no cols/rows
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);
    Column *c = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
    df->add_column(c, new String("name"));
    // now adding another column to nonempty dfs
    Column *c2 = new BoolColumn(8, true, false, true, false, true, false, true, false);
    df->add_column(c2, new String("2nd col"));
    Column *c3 = new FloatColumn(8, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22);
    df->add_column(c3, new String("3rd"));
    Column *c4 = new StringColumn(8, new String("h"), new String("e"), new String("l"), new String("l"), new String("o"), new String("w"), new String("o"), new String("r"));
    df->add_column(c4, new String("4th"));
    CS4500_ASSERT_TRUE(df->get_schema().coltypes_->equals((new String("IBFS"))));
    String *expected;
    char *ch;
    String *all = new String("helloworld");
    for (int i = 0; i < 8; i++)
    {
        CS4500_ASSERT_TRUE(df->get_int(0, i) == (i + 1));
        CS4500_ASSERT_TRUE(df->get_bool(1, i) == (i % 2 == 0));
        CS4500_ASSERT_TRUE(df->get_float(2, i) - ((float)i + 1.22) < 0.001);
        CS4500_ASSERT_TRUE(df->get_float(2, i) - ((float)i + 1.22) > -0.001);
        ch = new char(all->at(i));
        expected = new String(ch);
        CS4500_ASSERT_TRUE(df->get_string(3, i)->equals(expected));
    }
    delete expected;
    delete ch;
    exit(0);
}

TEST(dataframe, test4) { CS4500_ASSERT_EXIT_ZERO(testGet4); }

/**
 * @brief Setting the different cells and then checking if we can get the expected value.
 * 
 */
void testSet5()
{
    // empty schema, no cols/rows
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);
    Column *c = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
    df->add_column(c, new String("name"));
    // now adding another column to nonempty dfs
    Column *c2 = new BoolColumn(8, true, false, true, false, true, false, true, false);
    df->add_column(c2, new String("2nd col"));
    Column *c3 = new FloatColumn(8, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22);
    df->add_column(c3, new String("3rd"));
    Column *c4 = new StringColumn(8, new String("h"), new String("e"), new String("l"), new String("l"), new String("o"), new String("w"), new String("o"), new String("r"));
    df->add_column(c4, new String("4th"));
    CS4500_ASSERT_TRUE(df->get_schema().coltypes_->equals((new String("IBFS"))));
    char *ch;
    String *all = new String("helloworld");
    String *toAdd;
    for (int i = 0; i < 8; i++)
    {
        df->set(0, i, (int)i * 2);
        df->set(1, i, i % 2 == 1);
        df->set(2, i, (float)(i + 2.22));
        ch = new char(all->at(i + 1));
        toAdd = new String(ch);
        df->set(3, i, toAdd);
    }
    String *expected;
    for (int i = 0; i < 8; i++)
    {
        CS4500_ASSERT_TRUE(df->get_int(0, i) == (i * 2));
        CS4500_ASSERT_TRUE(df->get_bool(1, i) == (i % 2 == 1));
        CS4500_ASSERT_TRUE(df->get_float(2, i) - ((float)i + 2.22) < 0.001);
        CS4500_ASSERT_TRUE(df->get_float(2, i) - ((float)i + 2.22) > -0.001);
        ch = new char(all->at(i + 1));
        expected = new String(ch);
        CS4500_ASSERT_TRUE(df->get_string(3, i)->equals(expected));
    }
    exit(0);
}

TEST(dataframe, test5) { CS4500_ASSERT_EXIT_ZERO(testSet5); }

/**
 * @brief Runs all Google Tests in this file
 */
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}