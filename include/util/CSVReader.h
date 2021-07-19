//
//Created by vincent on 08.07.2021.
// A simple csv reader
// copied from https://stackoverflow.com/a/30338543/15553180

#pragma once
#include <istream>
#include <string>
#include <vector>

namespace util {

    enum class CSVState {
        UnquotedField,
        QuotedField,
        QuotedQuote
    };

    class CSVReader {
    public:
        CSVReader(const std::string& file){
            std::ifstream data(file);

            if (data.fail()) {
                std::cout << "Could not open csv file: " << file << std::endl;
                return;
            }
            readCSV(data);
        }

        std::vector<std::vector<std::string>>& getTable(){
            return m_Table;
        }

    private:
        std::string m_FilePath;
        std::vector<std::vector<std::string>> m_Table;

        /// Read CSV file, Excel dialect. Accept "quoted fields ""with quotes"""
        void readCSV(std::istream &in) {
            m_Table.clear();
            std::string row;
            while (!in.eof()) {
                std::getline(in, row);
                if (in.bad() || in.fail()) {
                    break;
                }
                auto fields = readCSVRow(row);
                m_Table.push_back(fields);
            }
        }

        std::vector<std::string> readCSVRow(const std::string &row) {
            CSVState state = CSVState::UnquotedField;
            std::vector<std::string> fields{""};
            size_t i = 0; // index of the current field
            for (char c : row) {
                switch (state) {
                    case CSVState::UnquotedField:
                        switch (c) {
                            case ',': // end of field
                                fields.push_back("");
                                i++;
                                break;
                            case '"':
                                state = CSVState::QuotedField;
                                break;
                            default:
                                fields[i].push_back(c);
                                break;
                        }
                        break;
                    case CSVState::QuotedField:
                        switch (c) {
                            case '"':
                                state = CSVState::QuotedQuote;
                                break;
                            default:
                                fields[i].push_back(c);
                                break;
                        }
                        break;
                    case CSVState::QuotedQuote:
                        switch (c) {
                            case ',': // , after closing quote
                                fields.push_back("");
                                i++;
                                state = CSVState::UnquotedField;
                                break;
                            case '"': // "" -> "
                                fields[i].push_back('"');
                                state = CSVState::QuotedField;
                                break;
                            default:  // end of quote
                                state = CSVState::UnquotedField;
                                break;
                        }
                        break;
                }
            }
            return fields;
        }
    };

}