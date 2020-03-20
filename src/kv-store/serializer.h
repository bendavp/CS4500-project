
// based of our understanding from link below:
// http://www.cplusplus.com/forum/general/201675/
class Serializer
{
public:
    Serializer() {}

    void serialize_int(int val, char *buffer)
    {
        buffer[0] = (val & 0xff000000) >> 24;
        buffer[1] = (val & 0x00ff0000) >> 16;
        buffer[2] = (val & 0x0000ff00) >> 8;
        buffer[3] = (val & 0x000000ff);
    }

    int deserialize_int(char *int_buffer)
    {
        int val = 0;
        val |= ((int)int_buffer[0]) << 24;
        val |= ((int)int_buffer[1]) << 16;
        val |= ((int)int_buffer[2]) << 8;
        val |= ((int)int_buffer[3]);
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
        buffer[0] = ((long)val & 0xff000000) >> 24;
        buffer[1] = ((long)val & 0x00ff0000) >> 16;
        buffer[2] = ((long)val & 0x0000ff00) >> 8;
        buffer[3] = ((long)val & 0x000000ff);
    }

    float deserialize_float(char *float_buffer)
    {
        long val = 0;
        val |= ((long)float_buffer[0]) << 24;
        val |= ((long)float_buffer[1]) << 16;
        val |= ((long)float_buffer[2]) << 8;
        val |= ((long)float_buffer[3]);
        return (float)val;
    }
};