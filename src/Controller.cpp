#include <Controller.h>
#include <iostream>
#include <inOutFile.h>
#include <InstructionLine.h>
#include <OperationTable.h>
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
    std::vector<std::string> operandVec;
    std::vector<std::string> operationVec;
    std::vector<std::string> labelVec;
    std::vector<std::string> commentVec;

    int locctr = 0;
    LitTable litTab = LitTable();
    inOutFile file = inOutFile();
    std::vector<std::string> input = file.readFile("Reader.txt");
    std::vector<std::string> intermediateFile;
    OperandValidator opValid = OperandValidator();
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
    string error = instruction.getError();
    int type = instruction.getType();
    int format = instruction.getInstructionFormatType();
    bool startFlag = false;
    toUpper(&operation);

    if (error == "") {
        errorMessage="";
    } else {
        errorMessage=error;
    }
    ///add while comment
    while(type == TYPE_COMMENT_ONLY){
        operandVec.push_back(operand);
        operationVec.push_back(operation);
        labelVec.push_back(label);
        commentVec.push_back(instruction.getComment());

        intermediateFile.push_back(line);
        errorMessageArr.push_back(errorMessage);
        locctrArr.push_back(locctr);
        fileIterator++;
        InstructionLine instructionComment = InstructionLine(input[fileIterator]);
        label = instructionComment.getLabel();
        operand = instructionComment.getOperand();
        operation = instructionComment.getOperation();
        line = instructionComment.getLine();
        error = instructionComment.getError();
        type = instructionComment.getType();
        format = instructionComment.getInstructionFormatType();
        toUpper(&operation);

    }
    if(operation == "START"){
        locctr = atoi(operand.c_str());
        startingAdrr = locctr;
        startFlag = true;
        if (type == TYPE_WITH_LABEL) {
            symTab.addSymbol(&label, locctr, false);
        }
    }
    if(!startFlag){
        locctr = 0;
        startingAdrr = 0;
    }
    operandVec.push_back(operand);
    operationVec.push_back(operation);
    labelVec.push_back(label);
    commentVec.push_back(instruction.getComment());
    intermediateFile.push_back(line);
    errorMessageArr.push_back(errorMessage);
    locctrArr.push_back(locctr);
    locctrArr.push_back(locctr);
    fileIterator++;


    while(operation != "END" && fileIterator !=  input.size()){
        InstructionLine instruct = InstructionLine(input[fileIterator]);
        cout<<"Line :: "<<instruct.getLine()<<endl;
        cout<<"Label :: "<<instruct.getLabel()<<endl;
        label = instruct.getLabel();
        operand = instruct.getOperand();
        cout<<"Operand :: "<<instruct.getOperand()<<endl;
        operation = instruct.getOperation();
        cout<<"Operation :: "<<operation<<endl;
        line = instruct.getLine();
        type = instruct.getType();
        toUpper(&operation);
        format = instruct.getInstructionFormatType();
        error = instruction.getError();
         cout<<"Error :: "<<error<<endl;

        if(type != TYPE_COMMENT_ONLY){

            if (error != "") {
                errorMessage = instruct.getError();
                operandVec.push_back(operand);
                operationVec.push_back(operation);
                labelVec.push_back(label);
                commentVec.push_back(instruction.getComment());
                intermediateFile.push_back(line);
                errorMessageArr.push_back(errorMessage);
                locctrArr.push_back(locctr);
                fileIterator ++;
                continue;
            }
            if(operation == "END") {
                endFound = true;
                errorMessage = "";
                operandVec.push_back(operand);
                operationVec.push_back(operation);
                labelVec.push_back(label);
                commentVec.push_back(instruction.getComment());
                intermediateFile.push_back(line);
                errorMessageArr.push_back(errorMessage);
                locctrArr.push_back(locctr);
                break;
            }

            if(opTable.contains(operation)){
                    if(operation == "LDB" && baseFound){
                        endBaseFound = true;
                    }
                    if(format == 4){
                        locctr += 4;
                    } else{
                        format = opTable.getFormat(operation);
                        locctr += format;
                    }
            } else if (!dirs.contains(operation)) {
                errorMessage = "Opcode doesn't exist";
                operandVec.push_back(operand);
                operationVec.push_back(operation);
                labelVec.push_back(label);
                commentVec.push_back(instruction.getComment());
                intermediateFile.push_back(line);
                errorMessageArr.push_back(errorMessage);
                locctrArr.push_back(locctr);
                fileIterator ++;
                continue;
            }
            if((operation == "NOBASE" || operation == "LTORG")
                && type != TYPE_WITHOUT_LABEL_AND_OPERAND && type != TYPE_WITH_LABEL_WITHOUT_OPERAND){
                errorMessage = operation + " found with label or operand";
                operandVec.push_back(operand);
                operationVec.push_back(operation);
                labelVec.push_back(label);
                commentVec.push_back(instruction.getComment());
                intermediateFile.push_back(line);
                errorMessageArr.push_back(errorMessage);
                locctrArr.push_back(locctr);
                fileIterator ++;
                continue;
            }
            int operandType;
            if (type == TYPE_WITH_LABEL || type == TYPE_WITHOUT_LABEL) {
                operandType = opValid.getOperandType(operand);
                cout<<"Operand type "<<operandType<<endl;
                if (operandType == -1 || !opValid.isCompatible(operandType, operation, operand)) {
                    errorMessage = "operand is not valid";
                    operandVec.push_back(operand);
                    operationVec.push_back(operation);
                    labelVec.push_back(label);
                    commentVec.push_back(instruction.getComment());
                    intermediateFile.push_back(line);
                    errorMessageArr.push_back(errorMessage);
                    locctrArr.push_back(locctr);
                    fileIterator ++;

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
                    errorMessage = "duplicated label in symbol table";
                } else{
                    symTab.addSymbol(&label, locctr, operation == "EQU");
                }
            }

            if(operation == "WORD"){
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
                } else if (operandType == TYPE_SYMBOL_OPERAND
                    && symTab.containSymbol(&operand)){
                    locctr = symTab.getSymbolAddress(&operand);
                } else if (operandType == TYPE_WORD_OPERAND) {
                    locctr = atoi(operand.c_str());
                }
            } else if (operation == "EQU") {
                if ((operandType == TYPE_IMMEDITAE_SYMBOL
                    || operandType == TYPE_INDIRECT_SYMBOL
                    || operandType == TYPE_SYMBOL_OPERAND)
                    && !symTab.containSymbol(&operand)) {
                        errorMessage = "label is not defined before EQU";
                } else if (operandType == TYPE_EXPRESSION){
                    string symbolFromExp = getSymbol(operand);
                    if (!symTab.containSymbol(&symbolFromExp)) {
                        errorMessage = "label is not defined before EQU";
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
        operandVec.push_back(operand);
        operationVec.push_back(operation);
        labelVec.push_back(label);
        commentVec.push_back(instruction.getComment());
        intermediateFile.push_back(line);
        errorMessageArr.push_back(errorMessage);
        locctrArr.push_back(locctr);

        fileIterator ++;
        if(fileIterator == input.size()) {
           break;
        }
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
              errorMessage = "operand is not valid";
              errorMessageArr.push_back(errorMessage);
            }
        }
    }
    if(!ltorgFound){
        litTab.assignAddress(locctr);
    }
    if(!endFound){
        errorMessage = "End statement not found";
        operandVec.push_back(operand);
        operationVec.push_back(operation);
        labelVec.push_back(label);
        commentVec.push_back(instruction.getComment());
        intermediateFile.push_back(line);
        errorMessageArr.push_back(errorMessage);
        locctrArr.push_back(locctr);
    }

    file.writeFile(labelVec, operationVec, operandVec, commentVec, errorMessageArr, locctrArr, "trial.txt");
    symTab.printSymbolTable();
    litTab.printLiteralTable();

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
