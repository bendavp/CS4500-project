#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using SoRCoord = std::pair<unsigned int, unsigned int>;
using SoRCol = std::vector<std::optional<std::string>>;
using SoRRow = std::vector<std::optional<std::string>>;
using SoRData = std::vector<SoRCol>;

//! Represents the supported types of SoR data
enum class SoRType { BOOL, INT, FLOAT, STRING };

// useful print operators
std::ostream& operator<<(std::ostream& os, const SoRType& type);
std::ostream& operator<<(std::ostream& os,
                         const std::optional<SoRType>& optType);
std::ostream& operator<<(std::ostream& os,
                         const std::optional<std::string>& optStr);

namespace {
constexpr size_t READBUFFER_SIZE_BYTES = 1024 * 1024;
}  // namespace

/**
 * @brief A parser that handles interpreting schema-on-read data files
 *
 * Schema-on-read files consist of cells formatted as <[content]>, with rows of
 * data separated by line breaks. Content can either be booleans, integers,
 * floats, or strings with the most restrictive datatype taking precedence.
 * Whitespace is not allowed in strings unless the string is enclosed in quotes.
 * The first 500 lines of a SoR file are used to determine the data's schema,
 * with the most common datatype of each column used to determine the column's
 * content.
 */
class SoRParser {
   public:
    //! Constructs a SoRParser
    SoRParser();

    //! Initializes parser with set file and determines schema with up to the
    //! first 500 lines
    bool initialize(std::string& filename);

    //! Gets the type of the specified column
    SoRType getColType(unsigned int col);

    //! Returns the unparsed value at the given location if valid (cell is not
    //! empty), otherwise empty
    std::optional<std::string> getColIdx(unsigned int col, unsigned int offset);

    //! Returns the value at the given location represented as a string
    std::string getColIdxStr(unsigned int col, unsigned int offset);

    //! Checks if location has a value
    bool isMissingIdx(unsigned int col, unsigned int offset);

    //! Converts a string into a row of SoR cells
    SoRRow parseFields(std::string& row);

    void setData(const std::string& path);
    void setDataStart(unsigned int start);
    void setDataSize(unsigned int size);

   private:
    std::ifstream data;      //! Stream of the raw data file
    unsigned int dataStart;  //! Data start point in bytes from the start of the
                             //! file
    unsigned int dataSize;   //! The readable size of the data (only relevant if
                             //! less that file size)
    unsigned int numCols;    //! The number of columns for this schema
    size_t cacheOffset;      //! The index represented by the start of the cache
    std::vector<SoRType> colTypes;  //! The datatypes of each column
    SoRData cache;  //! Cached version of part of the file, determined by
                    //! CACHE_SIZE
    std::vector<std::streampos> seenCaches;  //! Stream positions of cache start
                                             //! locations in ascending order
    size_t filesize;                         //! The size of the file
    std::streampos dataEnd;  //! The end of the file, only relevant if dataSize
                             //! is smaller than the file size from the start
                             //! offset
    std::shared_ptr<std::array<char, READBUFFER_SIZE_BYTES>> readBuffer;

    //! Determines the schema of the file using a sample of raw lines
    void setSchema(std::vector<std::string>& sample);
    void readInitial();
    SoRType findColType(const SoRCol& col);
    void addRow(SoRRow& row);
    void seekRow(unsigned int offset);

    void balanceCache();
    void loadCache(size_t cacheNum);
    void fillCache();
    void clearCache();
};
