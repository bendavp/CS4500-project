#include "../src/kv-store/kv-store.h"
#include <math.h>

void testSerializingDeserializing()
{
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);
    Column *c1 = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
    Column *c2 = new BoolColumn(8, true, false, true, false, true, false, true, false);
    Column *c3 = new FloatColumn(8, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22);
    Column *c4 = new StringColumn(8, new String("h"), new String("e"), new String("l"), new String("l"), new String("o"), new String("w"), new String("o"), new String("r"));
    df->add_column(c1, nullptr);
    df->add_column(c2, nullptr);
    df->add_column(c3, nullptr);
    df->add_column(c4, nullptr);
    std::cout << "printing df now" << std::endl;
    df->print();
    std::cout << "finished printing df here" << std::endl;
    Value *v = new Value();
    v->encode(df);

    DataFrame *df2 = v->decode();

    std::cout << "printing df2 now" << std::endl;
    df2->print();
}

int main()
{
    Serializer s_ = Serializer();

    int n = INT_MAX;
    std::cout << n << '\n';
    char *encoded = new char[4];
    s_.serialize_int(n, encoded);
    int decoded_i = s_.deserialize_int(encoded);
    std::cout << decoded_i << '\n';

    bool b = true;
    std::cout << b << '\n';
    char *encoded2 = new char[4];
    s_.serialize_bool(b, encoded2);
    bool decoded_b = s_.deserialize_bool(encoded2);
    std::cout << decoded_b << '\n';

    bool b2 = false;
    std::cout << b2 << '\n';
    char *encoded22 = new char[4];
    s_.serialize_bool(b2, encoded22);
    bool decoded_b2 = s_.deserialize_bool(encoded22);
    std::cout << decoded_b2 << '\n';

    float f = 3.402823466 * pow(10, 38);
    std::cout << f << '\n';
    char *encoded3 = new char[4];
    s_.serialize_float(f, encoded3);
    float decoded_f = s_.deserialize_float(encoded3);
    std::cout << decoded_f << '\n';

    testSerializingDeserializing();

    size_t sz = 5;
    String **strArray = new String *[sz];
    strArray[0] = new String("hello");
    strArray[1] = new String("world");
    strArray[2] = new String("please");
    strArray[3] = new String("pass");
    strArray[4] = new String("me");
    size_t buff_sz = 0;
    for (int i = 0; i < sz; i++)
    {
        std::cout << strArray[i]->c_str() << ", ";
        buff_sz += strArray[i]->size() + 1;
    }
    char *encoded4 = new char[buff_sz];
    s_.serialize_StringArray(strArray, encoded4, sz);
    String **strArray2 = s_.deserialize_StringArray(encoded4, sz);
    for (int i = 0; i < sz; i++)
    {
        std::cout << strArray2[i]->c_str() << ", ";
    }
}
