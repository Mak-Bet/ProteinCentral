#ifndef TSV_PARSING_H
#define TSV_PARSING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

struct TSVData {
    std::vector<std::string> headers;
    std::vector<std::vector<std::string> > rows;
};

bool is_empty(std::ifstream& pFile) {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void parseTSVHeadersAndRows(std::istream& input_stream, TSVData& tsv_data) {
    std::string line;

    // Read the first line to get the headers
    if (std::getline(input_stream, line)) {
        std::stringstream lineStream(line);
        std::string token;
        std::set<std::string> header_set;
        while (std::getline(lineStream, token, '\t')) {
            if (!header_set.insert(token).second) {
                throw std::runtime_error("Column name repetition is not permitted in this program!");
            }
            tsv_data.headers.push_back(token);
        }
    }

    // Read the rest of the lines to get the rows
    while (std::getline(input_stream, line)) {
        std::vector<std::string> tokens;
        std::stringstream lineStream(line);
        std::string token;
        while (std::getline(lineStream, token, '\t')) {
            tokens.push_back(token);
        }
        if (tokens.size() != tsv_data.headers.size()) {
            throw std::runtime_error("Mismatch between number of headers and number of columns in a line");
        }
        tsv_data.rows.push_back(tokens);
    }
}

#endif // TSV_PARSING_H