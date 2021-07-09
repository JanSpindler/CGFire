//
//Created by vincent on 08.07.2021.
// A simple csv writer based of https://stackoverflow.com/a/40760972

#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace util {
    class CSVWriter {
    private:
        std::ofstream m_OutFile;
        bool m_IsFirst;
        const std::string m_Separator;
        const std::string m_EscapeSeq;
        const std::string m_SpecialChars;
    public:
        CSVWriter(const std::string filename, const std::string separator = ",")
                : m_OutFile(), m_IsFirst(true), m_Separator(separator), m_EscapeSeq("\""), m_SpecialChars("\"")
                {
            m_OutFile.exceptions(std::ios::failbit | std::ios::badbit);
            m_OutFile.open(filename);
        }

        ~CSVWriter() {
            m_OutFile.flush();
            m_OutFile.close();
        }

        void endrow() {
            m_OutFile << std::endl;
            m_IsFirst = true;
        }

        CSVWriter &operator<<(CSVWriter &(*val)(CSVWriter &)) {
            return val(*this);
        }

        CSVWriter &operator<<(const char *val) {
            return write(escape(val));
        }

        CSVWriter &operator<<(const std::string &val) {
            return write(escape(val));
        }

        template<typename T>
        CSVWriter &operator<<(const T &val) {
            return write(val);
        }

    private:
        template<typename T>
        CSVWriter &write(const T &val) {
            if (!m_IsFirst) {
                m_OutFile << m_Separator;
            } else {
                m_IsFirst = false;
            }
            m_OutFile << val; //if an error occurs here, make sure to use only the type defined above (especially convert enum to int!)
            return *this;
        }

        std::string escape(const std::string &val) {
            std::ostringstream result;
            result << '"';
            std::string::size_type to, from = 0u, len = val.length();
            while (from < len &&
                   std::string::npos != (to = val.find_first_of(m_SpecialChars, from))) {
                result << val.substr(from, to - from) << m_EscapeSeq << val[to];
                from = to + 1;
            }
            result << val.substr(from) << '"';
            return result.str();
        }
    };


    inline static CSVWriter &endrow(CSVWriter &file) {
        file.endrow();
        return file;
    }
}