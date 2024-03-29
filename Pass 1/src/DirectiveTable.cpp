#include <iostream>
#include <algorithm>
#include <include/DirectiveTable.h>

/**
 * Singleton instance.
 */
DirectiveTable *DirectiveTable::instance = 0;

/**
 * Constructs directive table.
 */
DirectiveTable::DirectiveTable() {
    initDirTable();
}

/**
 * Gets the singleton instance of directive table.
 * @return [instance to directive table].
 */
DirectiveTable *DirectiveTable::getInstance() {
    if (instance == 0)
        instance = new DirectiveTable();
    return instance;
}


void DirectiveTable::initDirTable() {
    dirTable.push_back("START");
    dirTable.push_back("BYTE");
    dirTable.push_back("WORD");
    dirTable.push_back("RESB");
    dirTable.push_back("RESW");
    dirTable.push_back("EQU");
    dirTable.push_back("END");
    dirTable.push_back("ORG");
    dirTable.push_back("LTORG");
    dirTable.push_back("USE");
    dirTable.push_back("BASE");
    dirTable.push_back("NOBASE");
}
void DirectiveTable:: toUpper(string* symbolName) {
        transform(symbolName->begin(), symbolName->end(), symbolName->begin(), ::toupper);
}

/**
 * Checks if directive table contains a certain directive (case insensitive).
 * @param  directive [Directive to be checked for the bottom of its existence in this futile universe.]
 * @return           [True if directive table contains this directive.]
 */
bool DirectiveTable::contains(std::string directive) {
    toUpper(&directive);
    if (std::find (dirTable.begin(), dirTable.end(), directive) != dirTable.end())
    {
        return true;
    }

    return false;
}
