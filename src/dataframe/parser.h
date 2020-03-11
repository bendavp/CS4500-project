#include <cstring>
#include <iostream>
#include "dataframe.h"
// put dataframe stuff into the project folder and import column and dataframe

/**
 * @brief reads datafile.txt into a dataframe
 * 
 * @cite https://en.cppreference.com/w/cpp/string/byte/strtok
 * @cite https://riptutorial.com/c/example/8274/get-lines-from-a-file-using-getline--
 * 
 * @return int exit status
 */
DataFrame *parse_file(char *filename, int num_rows)
{
    char *line_buf = NULL;                              // buffer to store line contents, initialy NULL since we haven't read a line yet
    size_t line_buf_size = 0;                           // size of line buffer, initially zero since we haven't read a line yet
    size_t line_size;                                   // initialize size of line (i.e. number of chars)
    FILE *fp = fopen(filename, "r");                    // open datafile.txt
    line_size = getline(&line_buf, &line_buf_size, fp); // get first line of the file, set line_size to its size
    char *token = std::strtok(line_buf, "\t");          // set token to tokenize line

    Schema *s = new Schema("IIIFFBBSSS");
    DataFrame *df = new DataFrame(*s);
    Row *r = new Row(df->get_schema());

    float fval;
    float fval2;
    bool bval;
    bool bval2;

    for (size_t i = 0; i < num_rows; i++)
    {
        r->set(0, atoi(token));

        token = std::strtok(NULL, "\t");

        r->set(1, atoi(token));

        token = std::strtok(NULL, "\t");
        r->set(2, atoi(token));

        token = std::strtok(NULL, "\t");
        fval = atof(token);
        r->set(3, fval);

        token = std::strtok(NULL, "\t");
        fval2 = atof(token);
        r->set(4, fval2);

        token = std::strtok(NULL, "\t");
        bval = atoi(token);
        r->set(5, bval);

        token = std::strtok(NULL, "\t");
        bval2 = atoi(token);
        r->set(6, bval2);
        token = std::strtok(NULL, "\t");

        r->set(7, new String(token));
        token = std::strtok(NULL, "\t");

        r->set(8, new String(token));
        token = std::strtok(NULL, "\t");

        r->set(9, new String(token));

        df->add_row(*r);

        line_size = getline(&line_buf, &line_buf_size, fp); // get the next line (at end of file, line_size should be 0)

        token = std::strtok(line_buf, "\t"); // set token to tokenize next line
    }

    delete r;

    fclose(fp);

    return df;
}