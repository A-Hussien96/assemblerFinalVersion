#include "Controller.h"
#include <iostream>
#include <inOutFile.h>
#include <InstructionLine.h>
#include <OperationTable.h>
#include <DirectiveTable.h>
#include <vector>
#include <OperandValidator.h>
#include <SymbolTable.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

using namespace std;



Controller::Controller()
{

}
void Controller :: start()
{
    std::vector<std::string> errorMessageArr;
    std::vector<int> locctrArr;

    int locctr;
    LitTable litTab = LitTable();
    inOutFile file = inOutFile();
    std::vector<std::string> input = file.readFile("Reader.txt");
    std::vector<std::string> intermediateFile;
    OperandValidator opValid;
    DirectiveTable dirs;
    dirs.getInstance();
    OperationTable opTable;
    opTable.getInstance();
    SymbolTable symTab;

    /**/
    int startingAdrr = 0;
    string errorMessage;
    int fileIterator = 0;
    InstructionLine instruction = InstructionLine(input[fileIterator]);
    string label = instruction.getLabel();
    string operand = instruction.getOperand();
    string operation = instruction.getOperation();
    string line = instruction.getLine();
    int type = instruction.getType();
    bool startFlag = false;
    if(operation == "start"){
        locctr = atoi(operand.c_str());
        startingAdrr = locctr;
        startFlag = true;
        intermediateFile.push_back(line);
        cout<<line << endl;
    }
    if(!startFlag){
        locctr = 0;
    } else{
        fileIterator = 1;
        InstructionLine ins = InstructionLine(input[fileIterator]);
        label = ins.getLabel();
        operand = ins.getOperand();
        operation = ins.getOperation();
        line = ins.getLine();
        type = ins.getType();
    }
    toUpper(&operation);

    while(operation != "END" || fileIterator ==  input.size()){  ///or end of file
        if(type != TYPE_COMMENT_ONLY){
            if((operation == "NOBASE" || operation == "LTORG")
                && type != TYPE_WITHOUT_LABEL_AND_OPERAND && type != TYPE_WITH_LABEL_WITHOUT_OPERAND){
                errorMessage = "error in operand";
                continue;
            }
            int operandType;
            if (type == TYPE_WITH_LABEL || type == TYPE_WITHOUT_LABEL) {
                operandType = opValid.getOperandType(operand);
                if (operandType == -1 || !opValid.isCompatible(operandType, operation, operand)) {
                    errorMessage = "error in operand";
                    continue;
                }
                if (operandType == TYPE_HEXA_LITERAL ||
                    operandType == TYPE_BYTE_LITERAL ||
                    operandType == TYPE_WORD_LITERAL) {
                    if(!litTab.contain(operand)){
                        litTab.addLiteral(operand, operandType);
                    }
                }
            }
            if(type == TYPE_WITH_LABEL || type == TYPE_WITH_LABEL_WITHOUT_OPERAND){

                if(symTab.containSymbol(&label)){
                    errorMessage = "Symbol already found";
                } else{
                    symTab.addSymbol(&label, locctr, operation == "EQU");
                }
            }
            if(opTable.contains(operation)){
                if(operation == "LDB" && baseFound){
                    endBaseFound = true;
                }
                //inc locctr
            } else if (!dirs.contains(operation)) {
                errorMessage = "Opcode doesn't exist";
            }

            if(operation == "WOED"){
                locctr += 3;
            } else if(operation == "RESW"){
                locctr += 3 * atoi(operand.c_str());
            } else if(operation == "RESB"){
                locctr += atoi(operand.c_str());
            } else if(operation == "BYTE"){
                if (operandType == TYPE_HEXA_OPERAND) {
                    locctr += (operand.length() - 3)/2;
                } else if (operandType == TYPE_BYTE_OPERAND) {
                    locctr += (operand.length() - 3);
                }
            } else if (operation == "ORG") {
                if (operandType == TYPE_SYMBOL_OPERAND
                    && !symTab.containSymbol(&operand)) {
                        errorMessage = "error in operand";
                } else {
                    locctr = symTab.getSymbolAddress(&operand);
                }
            } else if (operation == "EQU") {
                if ((operandType == TYPE_IMMEDITAE_SYMBOL
                    || operandType == TYPE_INDIRECT_SYMBOL
                    || operandType == TYPE_SYMBOL_OPERAND)
                    && !symTab.containSymbol(&operand)) {
                        errorMessage = "error in operand";
                } else if (operandType == TYPE_EXPRESSION){
                    string symbolFromExp = getSymbol(operand);
                    if (!symTab.containSymbol(&symbolFromExp)) {
                        errorMessage = "error in operand";
                    }
                }
            } else if (operation == "LTORG") {
                litTab.assignAddress(locctr);
                ltorgFound = true;
            } else if (operation == "BASE") {
                baseFound = true;
            }
        }

        //handle if endBase = false; then error in line of base or load base
        intermediateFile.push_back(line);
        cout<<line << endl;
        fileIterator ++;
        InstructionLine instruct = InstructionLine(input[fileIterator]);
        label = instruct.getLabel();
        operand = instruct.getOperand();
        operation = instruct.getOperation();
        line = instruct.getLine();
        type = instruct.getType();
        errorMessageArr[fileIterator] = errorMessage;
        locctrArr[fileIterator] = locctr;
    }

    if(baseFound && !endBaseFound){
        errorMessage = "base not closed";
        errorMessageArr.push_back(errorMessage);
    }
    if (operation == "END") {
        if (baseFound && !baseFound) {
            errorMessage = "Base register is not identified";
            errorMessageArr.push_back(errorMessage);
        }
        if (type == TYPE_WITH_LABEL || type == TYPE_WITH_LABEL_WITHOUT_OPERAND) {
            if (symTab.containSymbol(&label)) {
                symTab.addSymbol(&label, locctr, false);
            } else {
                errorMessage = "duplicate symbol";
                errorMessageArr.push_back(errorMessage);
            }
        }
        if (type == TYPE_WITH_LABEL || type == TYPE_WITHOUT_LABEL) {
            int operandType = opValid.getOperandType(operand);
            if(operandType == -1 || !opValid.isCompatible(operandType, operation, operand)) {
              errorMessage = "error in operand";
              errorMessageArr.push_back(errorMessage);
            }
        }
    }
    if(!ltorgFound){
        litTab.assignAddress(locctr);
    }
    ///set end address by symtab.getAddress(operand);
    intermediateFile.push_back(line);
    cout<<line << endl;
    intermediateFile.push_back(static_cast<ostringstream*>( &(ostringstream() << locctr - startingAdrr) )->str());
    intermediateFile.push_back(static_cast<ostringstream*>( &(ostringstream() << startingAdrr) )->str());
    cout<<locctr - startingAdrr << endl;

}

string Controller::getSymbol(string expression) {
    std::size_t pos = expression.find("+");
    std::string symbolInExpression = expression.substr(0,pos);

    if (expression == symbolInExpression) {
        pos = expression.find("-");
        symbolInExpression = expression.substr(0,pos);
    }
    return symbolInExpression;
}
void Controller::toUpper(string* symbolName) {
        transform(symbolName->begin(), symbolName->end(), symbolName->begin(), ::toupper);
}
