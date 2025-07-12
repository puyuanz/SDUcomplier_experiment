#include<bits/stdc++.h>
using namespace std;

const map<string, string> keywords = {
    {"int","INTSYM"},
    {"double","DOUBLESYM"},
    {"scanf","SCANFSYM"},
    {"printf","PRINTFSYM"},
    {"if","IFSYM"},
    {"then","THENSYM"},
    {"while","WHILESYM"},
    {"do","DOSYM"}
};
const map<string, string> symbols = {
    {"=","AO"},
    {"==","RO"},
    {">","RO"},
    {">=","RO"},
    {"<","RO"},
    {"<=","RO"},
    {"||","LO"},
    {"&&","LO"},
    {"!","LO"},
    {"!=","RO"},
    {"+","PLUS"},
    {"-","MINUS"},
    {"*","TIMES"},
    {"/","DIVISION"},
    {",","COMMA"},
    {"(","BRACE"},
    {")","BRACE"},
    {"{","BRACE"},
    {"}","BRACE"},
    {";","SEMICOLON"}
};

string input;

void preProcess() {
    string temp;
    bool in_block_comment = false;
    bool in_line_comment = false;
    bool prev_was_space = false;
    int i = 0;
    int n = input.size();

    while (i < n) {
        if (in_block_comment) {
            // 处理块注释，直到找到 '*/'
            if (i + 1 < n && input[i] == '*' && input[i + 1] == '/') {
                in_block_comment = false;
                i += 2;
            }
            else {
                i++;
            }
        }
        else if (in_line_comment) {
            // 处理行注释，直到换行符
            if (input[i] == '\n') {
                in_line_comment = false;
                // 将换行符替换为空格，并处理合并
                if (!prev_was_space) {
                    temp += ' ';
                    prev_was_space = true;
                }
                i++;
            }
            else {
                i++;
            }
        }
        else {
            // 检查注释开始
            if (i + 1 < n && input[i] == '/' && input[i + 1] == '*') {
                in_block_comment = true;
                i += 2;
            }
            else if (i + 1 < n && input[i] == '/' && input[i + 1] == '/') {
                in_line_comment = true;
                i += 2;
            }
            else {
                // 处理普通字符
                char c = input[i];
                // 替换特殊字符为空格
                if (c == '\n' || c == '\t' || c == '\r') {
                    c = ' ';
                }
                // 合并连续空格
                if (c == ' ') {
                    if (!prev_was_space) {
                        temp += c;
                        prev_was_space = true;
                    }
                }
                else {
                    temp += c;
                    prev_was_space = false;
                }
                i++;
            }
        }
    }
    input = temp;
}

enum State {
    INIT,
    WORD,
    NUM,
};

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}
bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool canInword(char c) {
    return isLetter(c) || isDigit(c);
}
bool isSymbol(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '>' || c == '<' || c == '!' || c == '&' || c == '|';
}
bool isCanEquSymbol(char c) {
    return c == '=' || c == '>' || c == '<' || c == '!';
}
bool isMustRepeatSymbol(char c) {
    return (c == '&') || (c == '|');
}
bool isSingleSymbol(char c) {
    return (c == '{') || (c == '}') || (c == ';') || (c == '(') || (c == ')') || (c == ',') || (c == '+') || (c == '-') || (c == '*') || (c == '/');
}
list<pair<string, string> > ans;

void parse() {
    int n = input.size();
    int state;
    int Pos = 0;
    while (1) {
        // get the token to parse one by one
        string token;
        state = INIT;
        // escape the space
        while (Pos < n && input[Pos] == ' ')
            Pos++;
        while (Pos < n) {
            char c = input[Pos];
            bool if_break = false;
            switch (state) {
            case INIT: {
                if (isLetter(c))
                {
                    state = WORD;
                    token += c;
                }
                else if (isDigit(c) || c == '.')
                {
                    state = NUM;
                    token += c;
                }
                else if (isCanEquSymbol(c))
                {
                    token += c;
                    Pos++;
                    if (input[Pos] == '=')
                    {
                        token += input[Pos];
                        Pos++;
                    }
                    if_break = true;
                    break;
                }
                else if (isMustRepeatSymbol(c))
                {
                    token += c;
                    Pos++;
                    if (input[Pos] == input[Pos - 1])
                    {
                        token += c;
                        Pos++;
                    }
                    if_break = true;
                    break;
                }
                else if (isSingleSymbol(c))
                {
                    token += c;
                    Pos++;
                    if_break = true;
                    break;
                }
                else {
                    cout << "Unrecognizable characters.";
                    exit(0);
                }
                Pos++;
                break;
            }
            case WORD: {
                if (canInword(c))
                {
                    token += c;
                }
                else
                {
                    if_break = true;
                    break;
                }
                Pos++;
                break;
            }
            case NUM: {
                if (isDigit(c) || c == '.')
                {
                    token += c;
                }
                else
                {
                    if_break = true;
                    break;
                }
                Pos++;
                break;
            }
            }
            if (if_break) {
                break;
            }
        }
        if (token == "")
            break;
        //have got a token
        pair<string, string> temp;
        temp.first = token;
        if (keywords.find(token) != keywords.end()) {
            temp.second = keywords.at(token);
        }
        else if (symbols.find(token) != symbols.end()) {
            temp.second = symbols.at(token);
        }
        else if (isLetter(token[0])) {
            temp.second = "IDENT";
        }
        else if (isDigit(token[0]) || token[0] == '.') {
            if (token[0] == '.' || token[token.size() - 1] == '.') {
                cout << "Malformed number: Decimal point at the beginning or end of a floating point number.";
                exit(0);
            }
            if (token[0] == '0')
            {
                if (token.size() > 1)
                {
                    if (token[1] != '.')
                    {
                        cout << "Malformed number: Leading zeros in an integer.";
                        exit(0);
                    }
                }
            }
            bool hasDot = false;
            for (int i = 0; i < token.size(); i++) {
                if (token[i] == '.') {
                    if (hasDot) {
                        cout << "Malformed number: More than one decimal point in a floating point number.";
                        exit(0);
                    }
                    hasDot = true;
                }
                else if (!isDigit(token[i])) {
                    cout << "Malformed number: Non-digit character in a number.";
                    exit(0);
                }
            }
            if (hasDot) {
                temp.second = "DOUBLE";
            }
            else
                temp.second = "INT";
        }
        ans.push_back(temp);
    }
}

void getInput() {
    string line;
    while (getline(cin, line)) {
        input += line + '\n';
    }
}

int main() {
    getInput();
    preProcess();
    parse();
    for (auto i : ans) {
        cout << i.first << " " << i.second << endl;
    }
    return 0;
}