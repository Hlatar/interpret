#include "reader.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "../inc/printer.hpp"

int main(int argc, char* argv[]) {

    std::string buff = readfile(argc , argv);
    Lexer lexer(buff);
    std::vector<Token> tmp = lexer.tokenize();
    


    int i = 0;
    std::cout << "Lexer work:"<<std::endl;
    while(i < tmp.size()){
        
        std::cout <<tmp.at(i).toString()<< std::endl;
        i++;
        
    }
    
    Parser parser(tmp); // Создаём объект парсера с токенами
    auto ast = parser.parse(); // Вызываем метод parse для получения AST
    
    
    PrintVisitor smth;  // visitor дописать
    smth.print(*ast);

    return 0;
}

