#include "Pass2Controller.h"
#include <vector>
#include <string.h>
#include "OperandIdentifiers.h"
#include "DisplacementController.h"
#include "Format.h"
#include "Controller.h"
#include "IntermediateLine.h"
#include "inOutFile.h"
#include "Converters.h"
#include <OperandValidator.h>


Pass2Controller::Pass2Controller() {
    controller = Controller();
    format = Format();
    converter = Converters();
}

void Pass2Controller::generateIntermediateFile(string fileName) {
    controller.start(fileName);
}

void Pass2Controller::generateObjectCode() {
    int fileIterator = 2;
    file = inOutFile();
    string address;
    SymbolTable symbTab = controller.getSymbolTable();
    LitTable litTab = controller.getLitTable();
    std::vector<std::string> input = file.readFile("pass1.txt");
    IntermediateLine currentLine = IntermediateLine (input[fileIterator]);
    IntermediateLine nextLine = IntermediateLine (input[fileIterator]);
    cout << currentLine.getIntermediateLine();
    string start = currentLine.getAddress();
    string base = currentLine.getAddress();
    string progName = currentLine.getLabel();
    OperandIdentifiers operandIdentifiers = OperandIdentifiers(symbTab, litTab);
    displacementController = DisplacementController();
    int opType = OpValidator.getOperandType(currentLine.getOperand());
    operandIdentifiers.setOperand(currentLine.getOperand());
    while(fileIterator !=  input.size()) {
        string operand = currentLine.getOperand();
        string operation = currentLine.getOperation();
        if (currentLine.getOperation() == "BYTE") {
            objectCodeArr.push_back(getByteObjectCode(operand));
        } else if (currentLine.getOperation() == "WORD") {
            objectCodeArr.push_back(getWordObjectCode(operand));
        } else if (currentLine.getLabel() == "*") {
            objectCodeArr.push_back(getLiteralObjectCode(operand, litTab));
        }
        else if ((opType == TYPE_IMMEDITAE_SYMBOL || opType == TYPE_INDIRECT_SYMBOL ||
                 opType == TYPE_INDEXED_SYMBOL || opType == TYPE_SIMPLE_EXPRESSION ||
                 opType == TYPE_COMPLEX_EXPRESSION) && symbTab.getSymbolType(&operand) == 'U') {
                      	setSymbolType(operation, operand, opType, symbTab);
                if (symbTab.getSymbolType(&operand) == 'U') {
                        //error in line msh 3arfa hyt3ml ezay hna
                }
        } else {
            if (currentLine.getOperation() == "NOBASE") {
                displacementController.validBase = false;
            } else if (currentLine.getOperation() == "BASE") {
                base = currentLine.getAddress();
            }
            address = operandIdentifiers.getAddress();
            if (address == "") {
                if (!operandIdentifiers.isValidExpression()) {
                //then write error msg the expression is invalid
                } else {
                    //then symbol is not found in table error msg
                }
            } else {

            displacementController.setDispalcement(address, currentLine.getOperation(), currentLine.getOperand()
                                               , nextLine.getAddress(), base, currentLine.getFormat());
            format.setNflag(operandIdentifiers.getNflag());
            format.setIflag(operandIdentifiers.getIflag());
            format.setXflag(operandIdentifiers.getXflag());
            format.setBflag(displacementController.getBflag());
            format.setPflag(displacementController.getPCflag());
            format.setFormatType(currentLine.getFormat());
            format.setDispalcement(displacementController.getDispalcement());
            objectCodeArr.push_back(format.getObjectCode());
            }
        }
        fileIterator++;
        currentLine.setIntermediateLine(nextLine.getIntermediateLine());
        nextLine.setIntermediateLine(input[fileIterator]);
    }
}

void Pass2Controller::generateFinalFile(string start, string progName){
    file.writeHeaderFile (objectCodeArr, start, getLength(), progName);
}

string Pass2Controller::getLength()  {
    int counter = 0;
    for (int i = 0; i < objectCodeArr.size(); i++) {
        counter += objectCodeArr[i].length();
    }
    return converter.convertToHexa(counter/2);
}

string Pass2Controller::getByteObjectCode(string operand) {
    string code;
     for (int i = 3; i < operand.length() - 1; i++) {
        code.append(controller.getLitTable().getAscii(operand.at(i)));
    }
    return code;
}

string Pass2Controller::getWordObjectCode(string operand) {
    string code;
    if (operand.at(0) == '-') {
        int word = strtol(operand.c_str(), NULL, 16);
        word *= -1;
        code = converter.convertToHexa(displacementController.addBinary(displacementController.convertToBin(word), "1"));
    } else {
        code = converter.convertToHexa(strtol(operand.c_str(), NULL, 16));
    }
    return code;
}

string Pass2Controller::getLiteralObjectCode(string literal, LitTable litTab) {
    return litTab.getValue(literal);
}

void Pass2Controller::setSymbolType(string operation, string operand, int type, SymbolTable symbTab) {
    char newType = evaluateType(operation, operand, type, symbTab);
    symbTab.modifySymbol(&operand, symbTab.getSymbolAddress(&operand), newType);
}

char Pass2Controller::evaluateType(string operation, string operand, int type, SymbolTable symTab){
    if (operation == "WORD" || operation == "BYTE"
        || operation == "RESW" || operation == "RESB") {
        return 'R';
    } else if (type == TYPE_IMMEDITAE_WORD || type == TYPE_INDIRECT_WORD
               || type == TYPE_HEXA_LITERAL || type == TYPE_WORD_LITERAL
               || type == TYPE_BYTE_LITERAL) {

                return 'R';
    } else if (type == TYPE_IMMEDITAE_SYMBOL || type == TYPE_INDIRECT_SYMBOL) {
        string symbolName = operand.substr(1, operand.length() - 1);
         if (symTab.containSymbol(&symbolName)) {
            return symTab.getSymbolType(&symbolName);
        }
        return 'U';
    } else if (type == TYPE_SYMBOL_OPERAND) {
         if (symTab.containSymbol(&operand)) {
            return symTab.getSymbolType(&operand);
        }
        return 'U';
    } else if (type == TYPE_INDEXED_SYMBOL) {
        string symbolName = operand.substr(0, operand.length() - 2);
        if (symTab.containSymbol(&symbolName)) {
            return symTab.getSymbolType(&symbolName);
        }
        return 'U';
    } else if (type == TYPE_SIMPLE_EXPRESSION) {
        std::size_t sign = operand.find('+');
        if (sign > operand.length()) {
            sign = operand.find('-');
        }
        string operand1;
        if (isdigit(operand.at(0))) {
            operand1 = operand.substr(sign + 1, operand.length() - sign - 1);
        } else {
             operand1 = operand.substr(0, sign);
        }
        if (symTab.containSymbol(&operand1)) {
            return symTab.getSymbolType(&operand1);
        }
        return 'U';
    } else if (type == TYPE_COMPLEX_EXPRESSION) {
        std::size_t sign = operand.find('+');
        if (sign > operand.length()) {
            sign = operand.find('-');
        }
        string operand1 = operand.substr(sign + 1, operand.length() - sign - 1);
        string operand2 = operand.substr(0, sign);
        if ((symTab.getSymbolType(&operand1)=='A' && symTab.getSymbolType(&operand2) == 'A')
            || (symTab.getSymbolType(&operand1) == 'R' && symTab.getSymbolType(&operand2) == 'R' && sign == '-')) {
            return 'A';
        } else if ((symTab.getSymbolType(&operand1) == 'A' && symTab.getSymbolType(&operand2) == 'R' && sign == '+')
                   || (symTab.getSymbolType(&operand1) =='R' && symTab.getSymbolType(&operand2) == 'A')) {
            return 'R';
        }
        return 'U';
    }

}
