#ifndef EXCELWRITER_H
#define EXCELWRITER_H

#include <string>
#include <vector>

class ExcelWriter {
public:
    void writeSheet(
        const std::string& filePath,
        const std::vector<std::vector<std::string>>& data,
        const std::string& sheetName);
};

#endif
