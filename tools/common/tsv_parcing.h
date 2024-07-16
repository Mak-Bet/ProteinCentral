#ifndef TSV_PARSING_H
#define TSV_PARSING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

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
        while (std::getline(lineStream, token, '\t')) {
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
        tsv_data.rows.push_back(tokens);
    }
}

#endif // TSV_PARSING_H