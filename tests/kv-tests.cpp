#include "../src/kv-store/kv-store.h"
#include "../src/utils/rower.h"

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

    String *s = df->gmap(StrGeneratorDF());
    std::cout << s->c_str();

    Value *v = new Value();
    v->encode(df);

    std::cout << v->serialized_;

    DataFrame *df2 = v->decode(df2);
    String *s2 = df->gmap(StrGeneratorDF());
    std::cout << s2->c_str();

    assert(s.equals(s2));
}

int main()
{
    testSerializingDeserializing();
}

// serialize

// deserialize

// before serialization == after serialization AND deserialization