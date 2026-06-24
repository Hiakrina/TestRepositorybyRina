#ifndef EXCELREADER_H
#define EXCELREADER_H

#include <string>
#include <vector>

class ExcelReader {
public:
    std::vector<std::vector<std::string>> readSheet(
        const std::string& filePath,
        int sheetIndex);
};

#endif
