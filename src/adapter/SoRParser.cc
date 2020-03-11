#include <algorithm>
#include <exception>
#include <filesystem>
#include <sstream>

#include "OptParser.hh"
#include "SoRParser.hh"
#include "StrConverter.hh"

// Code forked from Boat
// https://github.com/gyroknight/boat-a1p1

namespace
{
constexpr int SCHEMA_DEF_LINES = 500;
constexpr int CACHE_SIZE = 512;
} // namespace

std::ostream &operator<<(std::ostream &os, const SoRType &type)
{
    switch (type)
    {
    case SoRType::BOOL:
        os << "BOOL";
        break;
    case SoRType::INT:
        os << "INT";
        break;
    case SoRType::FLOAT:
        os << "FLOAT";
        break;
    case SoRType::STRING:
        os << "STRING";
        break;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os,
                         const std::optional<SoRType> &optType)
{
    if (!optType.has_value())
        return os;
    return os << *optType;
}

std::ostream &operator<<(std::ostream &os,
                         const std::optional<std::string> &optStr)
{
    if (!optStr.has_value())
        return os;
    return os << *optStr;
}

SoRParser::SoRParser()
    : dataStart(0),
      dataSize(0),
      numCols(0),
      cacheOffset(0),
      readBuffer(std::make_shared<std::array<char, READBUFFER_SIZE_BYTES>>())
{
    data.rdbuf()->pubsetbuf(readBuffer->data(), readBuffer->size());
}
/**
 * @brief
 * @brief Initialize the parser
 *
 * @param filename the file
 * @return true if parser successfully initialized with file
 * @return false if not
 */
bool SoRParser::initialize(std::string &filename)
{
    try
    {
        setData(filename);
    }
    catch (std::ios_base::failure &e)
    {
        return false;
    }

    // Read first 500 lines, get schema
    readInitial();

    // Fill rest of cache
    fillCache();

    return true;
}

/**
 * @brief Get the type of a given column
 *
 * @param col the column
 * @return SoRType the type of the column
 */
SoRType SoRParser::getColType(unsigned int col) { return colTypes.at(col); }

std::optional<std::string> SoRParser::getColIdx(unsigned int col,
                                                unsigned int offset)
{
    if (col >= numCols)
        throw std::out_of_range("Invalid column");
    seekRow(offset);

    if (offset >= cacheOffset + cache.front().size())
        throw std::out_of_range("Invalid offset");
    return cache[col][offset % CACHE_SIZE];
}

std::string SoRParser::getColIdxStr(unsigned int col, unsigned int offset)
{
    SoRType colType = getColType(col);
    std::optional<std::string> value = getColIdx(col, offset);

    std::string ret;

    if (value.has_value())
    {
        switch (colType)
        {
        case SoRType::BOOL:
        case SoRType::INT:
            ret.append(std::to_string(std::stoi(*value)));
            break;
        case SoRType::FLOAT:
            ret.append(std::to_string(std::stof(*value)));
            break;
        case SoRType::STRING:
            ret.push_back('"');
            ret.append(*value).push_back('"');
            break;
        default:
            break;
        }
    }

    return ret;
}

/**
 * @brief Determines if a specific value is missing or not
 *
 * @param col the column
 * @param offset the row
 * @return std::optional<bool> true if missing, false if not. Empty if invalid
 * index.
 */
bool SoRParser::isMissingIdx(unsigned int col, unsigned int offset)
{
    return !getColIdx(col, offset).has_value();
}

/**
 * @brief Given a row of tag-deliminated values, (e.g. '<1>< 1.4 > <"hi"> <>'),
 * return a vector of the tags' fields. Spaces between tags and surrounding
 * values are trimmed.
 *
 * @param row the row to parse
 * @return std::vector<std::string> the vector of values
 */
SoRRow SoRParser::parseFields(std::string &row)
{
    // TODO(mike): Add support for "" as a valid string
    SoRRow fields;
    std::ostringstream field;

    // state machine states
    bool inField = false;
    bool inQuotes = false;
    bool fieldComplete = false;
    bool fieldEmpty = true;

    for (char c : row)
    {
        if (c == '<' && !inField)
        {
            // start of a tag
            inField = true;
        }
        else if (c == '>' && inField && !inQuotes)
        {
            // end of a tag
            inField = false;
            fieldComplete = false;
            if (!fieldEmpty || !field.str().empty())
                fields.push_back(std::make_optional(field.str()));
            else
                fields.push_back(std::nullopt);
            field.str("");
            fieldEmpty = true;
        }
        else if (c == '"')
        {
            // TODO add quotations to the field?
            if (inField && !inQuotes)
            {
                // start of quotes
                inQuotes = true;
                fieldEmpty = false;
            }
            else if (inQuotes && !fieldComplete)
            {
                // end of quotes
                inQuotes = false;
                fieldComplete = true;
            }
            else if (inField && fieldComplete)
            {
                // start of quotes but inside field
                fields.clear();
                return fields;
            }
        }
        else
        {
            // non-special characters or whitespace
            if (inField && !fieldComplete)
            {
                if (isspace(c) && !inQuotes)
                {
                    if (!field.str().empty())
                    {
                        // first trailing whitespace; no more characters allowed
                        // in this field
                        fieldComplete = true;
                    }
                }
                else
                {
                    // normal character
                    field << c;
                }
            }
            else if (inField && !isspace(c))
            {
                // got a non-whitespace character, but the field is already
                // done; invalid
                fields.clear();
                return fields;
            }
        }
    }

    return fields;
}

void SoRParser::setData(const std::string &path)
{
    data.open(path);
    if (!data.is_open())
        throw std::ios_base::failure(
            (std::string("Unable to open ") + path).c_str());
    clearCache();
    seenCaches.clear();

    data.clear();
    data.seekg(0, std::ios_base::beg);
    seenCaches.push_back(data.tellg());

    filesize = std::filesystem::file_size(path);
    dataSize = filesize;
}

void SoRParser::setDataStart(unsigned int start)
{
    if (start >= filesize)
        throw std::out_of_range("Data start past EOF");

    clearCache();
    seenCaches.clear();

    data.clear();
    data.seekg(0, std::ios_base::beg);
    data.ignore(start);
    if (start != 0)
        data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    seenCaches.push_back(data.tellg());
    setDataSize(dataSize);
    dataStart = start;

    fillCache();
}

void SoRParser::setDataSize(unsigned int size)
{
    dataSize = size;
    if (dataStart + dataSize < filesize)
    {
        data.clear();
        data.seekg(0, std::ios_base::beg);
        data.ignore(dataStart + size);
        dataEnd = data.tellg();
        clearCache();
        data.seekg(seenCaches.front());
        fillCache();
    }
}
/**
 * @brief Figures out the schema from the given sample by reading the longest
 * line.
 *
 * @param sample the sample
 */
void SoRParser::setSchema(std::vector<std::string> &sample)
{
    for (std::string &line : sample)
    {
        // split line into fields (i.e. things between tags)
        SoRRow fields = parseFields(line);

        // use longest line for the schema
        numCols = numCols < fields.size() ? fields.size() : numCols;
        balanceCache(); // make cache rectangular

        addRow(fields);
    }

    numCols = cache.size();

    // assign types for each column
    colTypes.resize(numCols);
    for (unsigned int ii = 0; ii < numCols; ii++)
    {
        colTypes[ii] = findColType(cache[ii]);
    }
}

//! Reads and gets schema from first 500 lines of the file
void SoRParser::readInitial()
{
    if (!data.is_open())
        return;

    // fill vector with first 500 lines of the file
    std::vector<std::string> sample;
    for (int ii = 0; ii < SCHEMA_DEF_LINES; ii++)
    {
        if (data.eof())
            break;
        std::string line;
        std::getline(data, line, '\n');
        sample.push_back(line);
    }

    // use them to set schema
    setSchema(sample);
}

SoRType SoRParser::findColType(const SoRCol &col)
{
    SoRType colType = SoRType::BOOL; // default col type
    size_t endIdx;

    for (const std::optional<std::string> &cell : col)
    {
        if (!cell.has_value())
            continue;

        SoRType cellType;
        int cellInt;
        const std::string &cellVal = *cell;

        try
        {
            cellInt = stoi(cellVal, &endIdx);
            if (endIdx == cellVal.size() && (cellInt == 0 || cellInt == 1) &&
                cellVal.size() == 1)
            {
                // if cell is int, equals 0 or 1, and only has length of 1, it
                // is a bool
                cellType = SoRType::BOOL;
            }
            else if (endIdx == cellVal.size())
            {
                cellType = SoRType::INT;
            }
            else
            {
                // starts with a number but hit something else (. or char)
                stof(cellVal, &endIdx);
                if (endIdx == cellVal.size())
                    // entirely a float
                    cellType = SoRType::FLOAT;
                else
                    // just a string starting with a number
                    return SoRType::STRING;
            }
        }
        catch (const std::invalid_argument &e)
        {
            try
            {
                // Could be float that begins with .
                stof(cellVal, &endIdx);
                if (endIdx == cellVal.size())
                    // entirely a float
                    cellType = SoRType::FLOAT;
                else
                    // didn't completely convert to float
                    return SoRType::STRING;
            }
            catch (const std::invalid_argument &e)
            {
                // Not a float
                return SoRType::STRING;
            }
        }
        catch (const std::out_of_range &e)
        {
            cellType = SoRType::FLOAT;
        }

        // promote column type from BOOL -> INT -> FLOAT -> STRING
        colType = colType > cellType ? colType : cellType;
    }

    return colType;
}

void SoRParser::addRow(SoRRow &row)
{
    // fill any missing fields at the end with empty strings (i.e. <>)
    while (row.size() < numCols)
        row.push_back(std::nullopt);

    // fill this column
    for (unsigned int ii = 0; ii < numCols; ii++)
        cache[ii].push_back(row[ii]);
}

/**
 * @brief Load cache that contains the specified column offset (row)
 *
 * @param offset The column offset (row)
 */
void SoRParser::seekRow(unsigned int offset)
{
    size_t cacheNum = offset / CACHE_SIZE;
    if (cacheNum * CACHE_SIZE != cacheOffset)
        loadCache(cacheNum);
}

/**
 * @brief Restores rectangularity to the cache - adds columns to cache until
 * they equal numCols, and then goes through each column and adds empty values
 * until every column is the same length as the first one.
 *
 */
void SoRParser::balanceCache()
{
    while (cache.size() < numCols)
    {
        cache.push_back(SoRCol());
    }

    size_t numRows = cache.front().size();
    for (SoRCol &col : cache)
    {
        while (col.size() < numRows)
        {
            col.push_back(std::nullopt);
        }
    }
}

/**
 * @brief Load the closest valid cache chunk to the cache number specified
 *
 * @param cacheNum The cache index
 */
void SoRParser::loadCache(size_t cacheNum)
{
    // Find matching cache location if possible
    if (cacheNum >= seenCaches.size())
    {
        data.clear();
        data.seekg(seenCaches.back());
        unsigned int seenRows = 0;
        data.clear();
        while (seenCaches.size() < cacheNum + 1 && !data.eof())
        {
            if (seenRows == CACHE_SIZE)
            {
                seenCaches.push_back(data.tellg());
                seenRows = 0;
            }

            data.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (dataStart + dataSize < filesize && data.tellg() >= dataEnd)
                break;
            seenRows++;
        }
    }

    if (cacheNum < seenCaches.size())
    {
        data.clear();
        data.seekg(seenCaches[cacheNum]);
        cacheOffset = cacheNum * CACHE_SIZE;
    }
    else
    {
        data.clear();
        data.seekg(seenCaches.back());
        cacheOffset = (seenCaches.size() - 1) * CACHE_SIZE;
    }

    clearCache();
    fillCache();
}

/**
 * @brief Fills cache with valid rows at current data position until either
 * CACHE_SIZE or end of data is reached
 */
void SoRParser::fillCache()
{
    while (cache.front().size() < CACHE_SIZE && !data.eof())
    {
        std::string line;
        std::getline(data, line, '\n');
        SoRRow row = parseFields(line);
        if (!data.eof() &&
            (dataStart + dataSize >= filesize || data.tellg() < dataEnd))
            addRow(row);
        else
            return;
    }
}

//! Clear the cache
void SoRParser::clearCache()
{
    for (SoRCol &col : cache)
    {
        col.clear();
    }
}

// implementation of functions added by Amy and Ben
// tang.amy@husky.neu.edu, kosiborod.b@husky.neu.edu
unsigned int SoRParser::getNumCols()
{
    return numCols;
}
