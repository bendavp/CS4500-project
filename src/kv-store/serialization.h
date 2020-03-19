class Serialization
{
public:
    Serialization() {}

    char *serialize_int(int val)
    {
        char buffer[4];
        buffer[0] = (val & 0xff000000) >> 24;
        buffer[1] = (val & 0x00ff0000) >> 16;
        buffer[2] = (val & 0x0000ff00) >> 8;
        buffer[3] = (val & 0x000000ff);
        return buffer;
    }

    int deserialize_int(char *int_buffer)
    {
        int val = 0;
        val |= ((int)int_buffer[0]) << 24;
        val |= ((int)int_buffer[1]) << 16;
        val |= ((int)int_buffer[2]) << 8;
        val |= ((int)int_buffer[3]);
    }
}