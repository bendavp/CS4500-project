//#include "../src/kv-store/kv-store.h"
#include "../src/kv-store/serializer.h"
#include <stdio.h>
#include <iostream>

// void testSerializingDeserializing()
// {
//     std::cout << "hello you piece of shit\n";
//     Schema *s = new Schema();
//     DataFrame *df = new DataFrame(*s);
//     std::cout << "success ln 8\n";
//     Column *c1 = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
//     Column *c2 = new BoolColumn(8, true, false, true, false, true, false, true, false);
//     std::cout << "success ln 11\n";
//     Column *c3 = new FloatColumn(8, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22);
//     Column *c4 = new StringColumn(8, new String("h"), new String("e"), new String("l"), new String("l"), new String("o"), new String("w"), new String("o"), new String("r"));
//     std::cout << "success ln 14\n";
//     df->add_column(c1, nullptr);
//     std::cout << "what\n";
//     df->add_column(c2, nullptr);
//     std::cout << "success added c1 c2\n";
//     df->add_column(c3, nullptr);
//     df->add_column(c4, nullptr);
//     std::cout << "success added c3 c4\n";
//     df->print();

//     Value *v = new Value();
//     v->encode(df);

//     std::cout << v->serialized_;

//     DataFrame *df2 = v->decode();

//     df2->print();
// }

int main()
{
    Serializer s_ = Serializer();

    int n = 10;
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

    float f = 1.22;
    std::cout << f << '\n';
    char *encoded3 = new char[4];
    s_.serialize_float(f, encoded3);
    float decoded_f = s_.deserialize_int(encoded3);
    std::cout << decoded_f << '\n';
    // testSerializingDeserializing();
}
