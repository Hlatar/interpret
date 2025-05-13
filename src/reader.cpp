#include "reader.hpp"

std::string readfile(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <имя_файла>" << std::endl;
        return "";
    }
    
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл!" << std::endl;
        return "";
    }

    int mem = CHAR_BUFF; 
    char* temp = new char[mem];  
    temp[0] = '\0';  

    int used = 0; 

    char buffer[CHAR_BUFF];  

    while (file.read(buffer, CHAR_BUFF - 1) || file.gcount() > 0) {
        buffer[file.gcount()] = '\0';  

        
        if (used + file.gcount() + 1 > mem) {
            mem *= 2;  
            char* newTemp = new char[mem];
            std::strcpy(newTemp, temp); 
            delete[] temp;
            temp = newTemp;
        }

        std::strcat(temp, buffer);  
        used += file.gcount();
    }

    std::string exit_string = temp;

    // std::cout << "Считанный текст:\n" << exit_string << std::endl;

    delete[] temp;
    file.close();
    return exit_string;
}