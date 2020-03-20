#include <string.h>
// based of our understanding from link below:
// http://www.cplusplus.com/forum/general/201675/
class Serializer
{
public:
    Serializer() {}

    void serialize_int(int val, char *buffer)
    {
        memcpy(buffer, &val, sizeof(int));
    }

    int deserialize_int(char *int_buffer)
    {
        int val;
        memcpy(&val, int_buffer, sizeof(int));
        return val;
    }

    void serialize_bool(bool val, char *buffer)
    {
        serialize_int((int)val, buffer);
    }

    bool deserialize_bool(char *bool_buffer)
    {
        bool val = (bool)deserialize_int(bool_buffer);
        return val;
    }

    void serialize_float(float val, char *buffer)
    {
        memcpy(buffer, &val, sizeof(float));
    }

    float deserialize_float(char *float_buffer)
    {
        float val;
        memcpy(&val, float_buffer, sizeof(float));
        return val;
    }
};