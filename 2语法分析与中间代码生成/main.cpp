#include<bits/stdc++.h>
using namespace std;
#define SYMINT 0
#define SYMDOUBLE 1
//文法中的所有单词
enum Type {
    START,
    ONE,
    INTSYM,
    DOUBLESYM,
    SCANFSYM,
    PRINTFSYM,
    IFSYM,
    THENSYM,
    WHILESYM,
    DOSYM,
    EQU,
    GREATER,
    GE,
    LESS,
    LE,
    OR,
    AND,
    NOT,
    NOTEQU,
    PLUS,
    MINUS,
    TIMES,
    DIVISION,
    COMMA,
    LBRACE1,
    RBRACE1,
    LBRACE2,
    RBRACE2,
    SEMICOLON,
    ID,
    IDENT,
    UDOUBLE,
    UINT,
    EQUEQU,
    PROG, SUBPROG,
    M,
    N,
    VARIABLES,
    STATEMENT,
    VARIABLE,
    T,
    ASSIGN,
    SCANF,
    PRINTF,
    L,
    B,
    EXPR,
    ORITEM,
    ANDITEM,
    RELITEM,
    NOITEM,
    ITEM,
    FACTOR,
    BORTERM,
    BANDTERM,
    BFACTOR,
    PLUS_MINUS,
    MUL_DIV,
    REL,
    SCANF_BEGIN,
    PRINTF_BEGIN,
    END,
};
vector<Type> allTypes = {
    Type::START,
    Type::ONE,
    Type::INTSYM,
    Type::DOUBLESYM,
    Type::SCANFSYM,
    Type::PRINTFSYM,
    Type::IFSYM,
    Type::THENSYM,
    Type::WHILESYM,
    Type::DOSYM,
    Type::EQU,
    Type::GREATER,
    Type::GE,
    Type::LESS,
    Type::LE,
    Type::OR,
    Type::AND,
    Type::NOT,
    Type::NOTEQU,
    Type::PLUS,
    Type::MINUS,
    Type::TIMES,
    Type::DIVISION,
    Type::COMMA,
    Type::LBRACE1,
    Type::RBRACE1,
    Type::LBRACE2,
    Type::RBRACE2,
    Type::SEMICOLON,
    Type::ID,
    Type::IDENT,
    Type::UDOUBLE,
    Type::UINT,
    Type::EQUEQU,
    Type::PROG,
    Type::SUBPROG,
    Type::M,
    Type::N,
    Type::VARIABLES,
    Type::STATEMENT,
    Type::VARIABLE,
    Type::T,
    Type::ASSIGN,
    Type::SCANF,
    Type::PRINTF,
    Type::L,
    Type::B,
    Type::EXPR,
    Type::ORITEM,
    Type::ANDITEM,
    Type::RELITEM,
    Type::NOITEM,
    Type::ITEM,
    Type::FACTOR,
    Type::BORTERM,
    Type::BANDTERM,
    Type::BFACTOR,
    Type::PLUS_MINUS,
    Type::MUL_DIV,
    Type::REL,
    Type::SCANF_BEGIN,
    Type::PRINTF_BEGIN,
    Type::END,
};
// 终结符号集合
set<int> endsymbolsSet = {
    Type::INTSYM,
    Type::DOUBLESYM,
    Type::SCANFSYM,
    Type::PRINTFSYM,
    Type::IFSYM,
    Type::THENSYM,
    Type::WHILESYM,
    Type::DOSYM,
    Type::EQU,
    Type::EQUEQU,
    Type::GREATER,
    Type::GE,
    Type::LESS,
    Type::LE,
    Type::OR,
    Type::AND,
    Type::NOT,
    Type::NOTEQU,
    Type::PLUS,
    Type::MINUS,
    Type::TIMES,
    Type::DIVISION,
    Type::COMMA,
    Type::LBRACE1,
    Type::RBRACE1,
    Type::LBRACE2,
    Type::RBRACE2,
    Type::SEMICOLON,
    Type::UDOUBLE,
    Type::UINT,
    Type::IDENT,
};
//高级语言单词对应的终结符号
map<string, Type> endsymbols = {
    {"int", INTSYM},
    {"double", DOUBLESYM},
    {"scanf", SCANFSYM},
    {"printf", PRINTFSYM},
    {"if", IFSYM},
    {"then", THENSYM},
    {"while", WHILESYM},
    {"do", DOSYM},
    {"=", EQU},
    {"==", EQUEQU},
    {">", GREATER},
    {">=", GE},
    {"<", LESS},
    {"<=", LE},
    {"||", OR},
    {"&&", AND},
    {"!", NOT},
    {"!=", NOTEQU},
    {"+", PLUS},
    {"-", MINUS},
    {"*", TIMES},
    {"/", DIVISION},
    {",", COMMA},
    {"(", LBRACE1},
    {")", RBRACE1},
    {"{", LBRACE2},
    {"}", RBRACE2},
    {";", SEMICOLON}
};

string inpu;				//输入的高级语言

//预处理删除多余空格、注释等
void preProcess() {
    string temp;
    bool in_block_comment = false;
    bool in_line_comment = false;
    bool prev_was_space = false;
    int i = 0;
    int n = inpu.size();

    while (i < n) {
        if (in_block_comment) {
            // 处理块注释，直到找到 '*/'
            if (i + 1 < n && inpu[i] == '*' && inpu[i + 1] == '/') {
                in_block_comment = false;
                i += 2;
            }
            else {
                i++;
            }
        }
        else if (in_line_comment) {
            // 处理行注释，直到换行符
            if (inpu[i] == '\n') {
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
            if (i + 1 < n && inpu[i] == '/' && inpu[i + 1] == '*') {
                in_block_comment = true;
                i += 2;
            }
            else if (i + 1 < n && inpu[i] == '/' && inpu[i + 1] == '/') {
                in_line_comment = true;
                i += 2;
            }
            else {
                // 处理普通字符
                char c = inpu[i];
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
    inpu = temp;
}

// 词法分析的三个状态
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
list<Type> ans;
list<string> numData;
void lex() {
    int n = inpu.size();
    int state;
    int Pos = 0;
    while (1) {
        // get the token to parse one by one
        string token;
        state = INIT;
        // escape the space
        while (Pos < n && inpu[Pos] == ' ')
            Pos++;
        while (Pos < n) {
            char c = inpu[Pos];
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
                    if (inpu[Pos] == '=')
                    {
                        token += inpu[Pos];
                        Pos++;
                    }
                    if_break = true;
                    break;
                }
                else if (isMustRepeatSymbol(c))
                {
                    token += c;
                    Pos++;
                    if (inpu[Pos] == inpu[Pos - 1])
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
        //此时获得了一个token单词，然后找对应文法的终结符号是什么
        pair<string, Type> temp;
        temp.first = token;
        if (endsymbols.find(token) != endsymbols.end()) {
            //找到终结符号
            temp.second = endsymbols.at(token);
        }
        else if (isLetter(token[0])) {
            //变量名
            temp.second = IDENT;
        }
        else if (isDigit(token[0]) || token[0] == '.') {
            //整数或者浮点数数据
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
                temp.second = UDOUBLE;
            }
            else
                temp.second = UINT;
        }
        ans.push_back(temp.second);
        if (temp.second == IDENT)
        {
            numData.push_back(temp.first);
        }
        else if (temp.second == UINT) {
            int num = stoi(temp.first);
            numData.push_back(to_string(num));
        }
        else if (temp.second == UDOUBLE) {
            double num = stod(temp.first);
            numData.push_back(to_string(num));
        }
    }
}
// 通过词法分析，我们得到了所有单词，以及对应的文法的终结符号
void getinpu() {
    string line;
    while (getline(cin, line)) {
        inpu += line + '\n';
    }
}

class Expression
{
public:
    Expression(Type left, vector<Type> right, function<void(void)> action) : left(left), right(right), action(action) {};
    Type left;
    vector<Type> right;
    function<void(void)> action;
};
class Symbol
{
public:
    Symbol()
    {
        rtype = Type::ONE;
    }
    Symbol(Type type) : rtype(type) {};
    Type rtype;
    string quad = "";
    string name = "";
    string nextList = "";
    string trueList = "";
    string falseList = "";
    int type = 0;
    int width = 0;
    string op = "";
    string place = "";
};
struct Quardruple {
    string op;
    string arg1;
    string arg2;
    string result;
    Quardruple(string op, string arg1, string arg2, string result) :op(op), arg1(arg1), arg2(arg2), result(result) {};
};
struct SymbolTableItem {
    string name;
    int type;
    string value;
    int offset;
    bool hasValue = false;
    SymbolTableItem() = default;
    SymbolTableItem(string name, int type, string value, int offset) :name(name), type(type), value(value), offset(offset) {};
};
struct ClosureItem
{
    int expressionIndex;
    size_t dot;
    set<Type> lookAhead;
    bool operator<(const ClosureItem& item) const
    {
        if (expressionIndex != item.expressionIndex)
        {
            return expressionIndex < item.expressionIndex;
        }
        if (dot != item.dot)
        {
            return dot < item.dot;
        }
        return false;
    }
    bool operator==(const ClosureItem& item) const
    {
        return (!(item < *this)) && (!(*this < item));
    }
};
typedef set<ClosureItem> I;
struct CCompare {
    bool operator() (const pair<I, int>& a, const pair<I, int>& b) const {
        return a.first < b.first;
    }
};
struct StateTrans
{
    int oldState;
    Type go;
    int newState;
    StateTrans(int oldState, Type go, int newState) :oldState(oldState), go(go), newState(newState) {};
    bool operator<(const StateTrans& trans) const
    {
        if (this->oldState != trans.oldState) {
            return this->oldState < trans.oldState;
        }
        else if (this->go != trans.go) {
            return this->go < trans.go;
        }
        return this->newState < trans.newState;
    }
};

class Parser {
public:
    Parser();
    void run();
    void output();
private:
    int OFFSET = 0;
    int nxq = 0;
    int tempIndex = 0;
    bool isEndSymbol(Type type) {
        return endsymbolsSet.find(type) != endsymbolsSet.end();
    }

    map<Type, set<Type> > first;
    set<Type> getFirstForCandidate(vector<Type> candidiate);

    map<Type, set<Type> > follow;

    // to finish the table
    set<StateTrans> StateTransSet;
    I Clousre(ClosureItem sourceItem);
    I Go(I i, Type a);
    set<pair<I, int>, CCompare> C;
    map<int, I> int2I;
    std::map<int, map<Type, function<void(void)>>> analyticalTable;

    vector<Expression> grammar;
    map<Type, vector<int> > grammarIndex;

    list<Symbol> symbolStack;
    stack<int> stateStack;
    list<Type> input;
    vector<Quardruple> QuardrupleTable;
    list<string> numAndId;

    map<string, int> symbolTableIndex;
    vector<SymbolTableItem> symbolTable;

    void backpatch(string p, string b);
    string merge(string p1, string p2);
    void gen(string op, string arg1, string arg2, string result);
    void enter(string name, int type, int offset);
    string mklist(int i);
    string mklist();
    int lookup_type(string name);
    string lookup(string name);
    string newTemp(int type);
};
I Parser::Clousre(ClosureItem sourceItem)
{
    I result;
    queue<ClosureItem> q;
    q.push(sourceItem);
    result.insert(sourceItem);
    while (!q.empty()) {
        ClosureItem item = q.front();
        q.pop();
        // result.insert(item);
        if (item.dot < grammar[item.expressionIndex].right.size()) {
            Type nextType = grammar[item.expressionIndex].right[item.dot];
            if (!isEndSymbol(nextType)) {
                for (auto& exprIt : grammarIndex[nextType]) {
                    auto expressionIndex = exprIt;
                    ClosureItem newItem = { expressionIndex,0,{} };
                    if (item.dot == grammar[item.expressionIndex].right.size() - 1) {
                        newItem.lookAhead.insert(item.lookAhead.begin(), item.lookAhead.end());
                    }
                    else {
                        set<Type> firstSet = first[grammar[item.expressionIndex].right[item.dot + 1]];
                        newItem.lookAhead.insert(firstSet.begin(), firstSet.end());
                    }
                    auto res = result.insert(newItem);
                    if (res.second) {
                        q.push(newItem);
                    }
                    else if (!res.second) {
                        if (res.first->lookAhead != newItem.lookAhead) {
                            auto temp = *res.first;
                            result.erase(res.first);
                            bool changed = false;
                            for (auto& type : newItem.lookAhead) {
                                auto res2 = temp.lookAhead.insert(type);
                                if (res2.second) {
                                    changed = true;
                                }
                            }
                            if (changed) {
                                result.insert(temp);
                                q.push(temp);
                            }
                            else {
                                result.insert(temp);
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}
I Parser::Go(I i, Type a)
{
    I result;
    for (auto& item : i) {
        if (item.dot < grammar[item.expressionIndex].right.size() && grammar[item.expressionIndex].right[item.dot] == a) {
            ClosureItem newItem = { item.expressionIndex,item.dot + 1,item.lookAhead };
            I closure = Clousre(newItem);
            result.insert(closure.begin(), closure.end());
        }
    }
    return result;
}


//通过传给右边的candidate得到对应这个组的first
set<Type> Parser::getFirstForCandidate(vector<Type> candidate)
{
    set<Type> result;
    if ((candidate.size() == 1) && (candidate[0] == ONE))
    {
        result.insert(ONE);
        return result;
    }
    result = first[candidate[0]];
    result.erase(ONE);
    bool isEmpty = true;
    for (int i = 1; i < (int)candidate.size(); i++)
    {
        if (first[candidate[i - 1]].find(ONE) != first[candidate[i - 1]].end())
        {
            //如果这一个有空符号，则下一个的first也可以作为这个组的first，即i++后continue；
            result.insert(first[candidate[i]].begin(), first[candidate[i]].end());
            result.erase(ONE);
        }
        else
        {
            isEmpty = false;
            break;
        }
    }
    if (isEmpty && first[candidate[candidate.size() - 1]].find(ONE) != first[candidate[candidate.size() - 1]].end())
    {
        result.insert(ONE);
    }
    return result;
}
// 回填空目标地址链
void Parser::backpatch(string p, string b) {
    string q;
    while (p != "null" && p != "0") {
        q = QuardrupleTable[stoi(p)].result;
        QuardrupleTable[stoi(p)].result = b;
        p = q;
    }
}
// 合并两个空目标地址链，往往是truelist或者falselist
string Parser::merge(string p1, string p2) {
    if (p1 == "null") {
        return p2;
    }
    if (p2 == "null") {
        return p1;
    }
    string p = p2;
    while (QuardrupleTable[stoi(p)].result != "null" && QuardrupleTable[stoi(p)].result != "0") {
        p = QuardrupleTable[stoi(p)].result;
    }
    QuardrupleTable[stoi(p)].result = p1;
    return p2;
}
//生成四元式
void Parser::gen(string op, string arg1, string arg2, string result) {
    if ((int)QuardrupleTable.size() < nxq + 1) {
        QuardrupleTable.push_back(Quardruple(op, arg1, arg2, result));
    }
    else {
        QuardrupleTable[nxq] = Quardruple(op, arg1, arg2, result);
    }
    nxq++;
}
//写入符号表
void Parser::enter(string name, int type, int offset) {
    if (symbolTableIndex.find(name) != symbolTableIndex.end()) {
        cout << "Syntax Error" << endl;
        exit(0);
    }
    SymbolTableItem item(name, type, "null", offset);
    symbolTable.push_back(item);
    symbolTableIndex[name] = symbolTable.size() - 1;
}
//创建一个list
string Parser::mklist(int i)
{
    return to_string(i);
}

string Parser::mklist()
{
    return "null";
}

int Parser::lookup_type(string name)
{
    if (symbolTableIndex.find(name) != symbolTableIndex.end()) {
        return symbolTable[symbolTableIndex[name]].type;
    }
    return -1;
}

string Parser::lookup(string name)
{
    if (symbolTableIndex.find(name) != symbolTableIndex.end()) {
        return "TB" + to_string(symbolTableIndex[name]);
    }
    cout << "Syntax Error" << endl;
    exit(0);
    return "null";
}

string Parser::newTemp(int type)
{
    string name = "T" + to_string(tempIndex++);
    if (type == SYMINT) {
        name += "_i";
    }
    else if (type == SYMDOUBLE) {
        name += "_d";
    }
    return name;
}


Parser::Parser() {
    this->numAndId = numData;
    this->input = ans;
    this->input.push_back(END);

    grammar = {
        //文法实现：根据题目生成对应代码
        {START,{PROG},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(START);
            symbolStack.push_back(symbol);
        }},
        {PROG,{SUBPROG},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(PROG);
            symbolStack.push_back(symbol);
        }},
        {SUBPROG,{M,VARIABLES,STATEMENT},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto statement = symbolStack.back();
            symbolStack.pop_back();
            auto variables = symbolStack.back();
            symbolStack.pop_back();
            auto m = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(SUBPROG);
            backpatch(statement.nextList,to_string(nxq));
            gen("End","-","-","-");
            symbolStack.push_back(symbol);
        }},
        {M,{ONE},[&]() {
            OFFSET = 0;
            stateStack.pop();
            Symbol symbol(M);
            symbolStack.push_back(symbol);
        }},
        {N,{ONE},[&]() {
            stateStack.pop();
            Symbol symbol(N);
            symbol.quad = to_string(nxq);
            symbolStack.push_back(symbol);
        }},
        //变量声明部分
        {VARIABLES,{VARIABLES,VARIABLE,SEMICOLON},[&]() {
            int repeat = 3;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(VARIABLES);
            symbolStack.push_back(symbol);
        }},
        {VARIABLES,{VARIABLE,SEMICOLON},[&]() {
            int repeat = 2;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(VARIABLES);
            symbolStack.push_back(symbol);
        }},
        {T,{INTSYM},[&]() {
            symbolStack.pop_back();
            stateStack.pop();
            Symbol symbol(T);
            symbol.type = SYMINT;
            symbol.width = 4;
            symbolStack.push_back(symbol);
        }},
        {T,{DOUBLESYM},[&]() {
            symbolStack.pop_back();
            stateStack.pop();
            Symbol symbol(T);
            symbol.type = SYMDOUBLE;
            symbol.width = 8;
            symbolStack.push_back(symbol);
        }},
        {ID,{IDENT},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(ID);
            symbol.name = numAndId.front();
            numAndId.pop_front();
            symbolStack.push_back(symbol);
        }},
        {VARIABLE,{T,ID},[&]() {
            int repeat = 2;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            auto t = symbolStack.back();
            symbolStack.pop_back();
            enter(id.name,t.type,OFFSET);
            OFFSET += t.width;
            Symbol symbol(VARIABLE);
            symbol.type = t.type;
            symbol.width = t.width;
            symbolStack.push_back(symbol);
        }},
        {VARIABLE,{VARIABLE,COMMA,ID},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto varible = symbolStack.back();
            symbolStack.pop_back();
            enter(id.name,varible.type,OFFSET);
            OFFSET += varible.width;
            Symbol symbol(VARIABLE);
            symbol.type = varible.type;
            symbol.width = varible.width;
            symbolStack.push_back(symbol);
        }},
        //语句部分
        {STATEMENT,{ASSIGN},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(STATEMENT);
            symbol.nextList = mklist();
            symbolStack.push_back(symbol);
        }},
        {STATEMENT,{SCANF},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(STATEMENT);
            symbol.nextList = mklist();
            symbolStack.push_back(symbol);
        }},
        {STATEMENT,{PRINTF},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(STATEMENT);
            symbol.nextList = mklist();
            symbolStack.push_back(symbol);
        }},
        {STATEMENT,{ONE},[&]() {
            stateStack.pop();
            Symbol symbol(STATEMENT);
            symbol.nextList = mklist();
            symbolStack.push_back(symbol);
        }},
        {STATEMENT,{LBRACE2,L,SEMICOLON,RBRACE2},[&]() {
            int repeat = 4;
            while (repeat--) {
                stateStack.pop();
            }
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto l = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(STATEMENT);
            symbol.nextList = l.nextList;
            symbolStack.push_back(symbol);
        }},
        {STATEMENT,{WHILESYM,N,B,DOSYM,N,STATEMENT},[&]() {
            int repeat = 6;
            while (repeat--) {
                stateStack.pop();
            }
            auto statement = symbolStack.back();
            symbolStack.pop_back();
            auto n2 = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto b = symbolStack.back();
            symbolStack.pop_back();
            auto n1 = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(STATEMENT);
            backpatch(statement.nextList,n1.quad);
            backpatch(b.trueList,n2.quad);
            symbol.nextList = b.falseList;
            gen("j","-","-",n1.quad);
            symbolStack.push_back(symbol);
        }},
        {STATEMENT,{IFSYM,B,THENSYM,N,STATEMENT},[&]() {
            int repeat = 5;
            while (repeat--) {
                stateStack.pop();
            }
            auto statement = symbolStack.back();
            symbolStack.pop_back();
            auto n = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto b = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(STATEMENT);
            backpatch(b.trueList,n.quad);
            symbol.nextList = merge(b.falseList,statement.nextList);
            symbolStack.push_back(symbol);
        }},
        {ASSIGN,{ID,EQU,EXPR},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto expr = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto id = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(ASSIGN);
            auto p = lookup(id.name);
            gen("=",expr.place,"-",p);
            symbolStack.push_back(symbol);
        }},
        {L,{L,SEMICOLON,N,STATEMENT},[&]() {
            int repeat = 4;
            while (repeat--) {
                stateStack.pop();
            }
            auto statement = symbolStack.back();
            symbolStack.pop_back();
            auto n = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto l = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(L);
            backpatch(l.nextList,n.quad);
            symbol.nextList = statement.nextList;
            symbolStack.push_back(symbol);
        }},
        {L,{STATEMENT},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto statement = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(L);
            symbol.nextList = statement.nextList;
            symbolStack.push_back(symbol);
        }},
        //数值表达式部分
        {EXPR,{EXPR,OR,ORITEM},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto orItem = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto expr = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(EXPR);
            symbol.place = newTemp(SYMINT);
            symbol.type = SYMINT;
            gen("||",expr.place,orItem.place,symbol.place);
            symbolStack.push_back(symbol);
        }},
        {EXPR,{ORITEM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto orItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(EXPR);
            symbol.place = orItem.place;
            symbol.type = orItem.type;
            symbolStack.push_back(symbol);
        }},
        {ORITEM,{ORITEM,AND,ANDITEM},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto andItem = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto orItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(ORITEM);
            symbol.place = newTemp(SYMINT);
            symbol.type = SYMINT;
            gen("&&",orItem.place,andItem.place,symbol.place);
            symbolStack.push_back(symbol);
        }},
        {ORITEM,{ANDITEM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto andItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(ORITEM);
            symbol.place = andItem.place;
            symbol.type = andItem.type;
            symbolStack.push_back(symbol);
        }},
        {ANDITEM,{NOITEM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto noItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(ANDITEM);
            symbol.place = noItem.place;
            symbol.type = noItem.type;
            symbolStack.push_back(symbol);
        }},
        {ANDITEM,{NOT,NOITEM},[&]() {
            int repeat = 2;
            while (repeat--) {
                stateStack.pop();
            }
            auto noItem = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(ANDITEM);
            symbol.place = newTemp(SYMINT);
            symbol.type = SYMINT;
            gen("!",noItem.place,"-",symbol.place);
            symbolStack.push_back(symbol);
        }},
        {NOITEM,{NOITEM,REL,RELITEM},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto relItem = symbolStack.back();
            symbolStack.pop_back();
            auto rel = symbolStack.back();
            symbolStack.pop_back();
            auto noItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(NOITEM);
            symbol.place = newTemp(SYMINT);
            symbol.type = SYMINT;
            gen(rel.op,noItem.place,relItem.place,symbol.place);
            symbolStack.push_back(symbol);
        }},
        {NOITEM,{RELITEM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto relItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(NOITEM);
            symbol.place = relItem.place;
            symbol.type = relItem.type;
            symbolStack.push_back(symbol);
        }},
        {RELITEM,{RELITEM,PLUS_MINUS,ITEM},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto item = symbolStack.back();
            symbolStack.pop_back();
            auto plusMinus = symbolStack.back();
            symbolStack.pop_back();
            auto relItem = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(RELITEM);
            symbol.place = newTemp(relItem.type);
            symbol.type = relItem.type;
            gen(plusMinus.op,relItem.place,item.place,symbol.place);
            symbolStack.push_back(symbol);
        }},
        {RELITEM,{ITEM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto item = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(RELITEM);
            symbol.place = item.place;
            symbol.type = item.type;
            symbolStack.push_back(symbol);
        }},
        {ITEM,{FACTOR},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto factor = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(ITEM);
            symbol.place = factor.place;
            symbol.type = factor.type;
            symbolStack.push_back(symbol);
        }},
        {ITEM,{ITEM,MUL_DIV,FACTOR},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto factor = symbolStack.back();
            symbolStack.pop_back();
            auto mulDiv = symbolStack.back();
            symbolStack.pop_back();
            auto item = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(ITEM);
            symbol.place = newTemp(factor.type);
            symbol.type = factor.type;
            gen(mulDiv.op,item.place,factor.place,symbol.place);
            symbolStack.push_back(symbol);
        }},
        {FACTOR,{ID},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(FACTOR);
            symbol.place = lookup(id.name);
            symbol.type = lookup_type(id.name);
            symbolStack.push_back(symbol);
        }},
        {FACTOR,{UINT},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(FACTOR);
            symbol.place = newTemp(SYMINT);
            symbol.type = SYMINT;
            gen("=",numAndId.front(),"-",symbol.place);
            numAndId.pop_front();
            symbolStack.push_back(symbol);
        }},
        {FACTOR,{UDOUBLE},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(FACTOR);
            symbol.place = newTemp(SYMDOUBLE);
            symbol.type = SYMDOUBLE;
            gen("=",numAndId.front(),"-",symbol.place);
            numAndId.pop_front();
            symbolStack.push_back(symbol);
        }},
        {FACTOR,{LBRACE1,EXPR,RBRACE1},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            symbolStack.pop_back();
            auto expr = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(FACTOR);
            symbol.place = expr.place;
            symbol.type = expr.type;
            symbolStack.push_back(symbol);
        }},
        {FACTOR,{PLUS_MINUS,FACTOR},[&]() {
            int repeat = 2;
            while (repeat--) {
                stateStack.pop();
            }
            auto factor = symbolStack.back();
            symbolStack.pop_back();
            auto plusMinus = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(FACTOR);
            symbol.place = newTemp(factor.type);
            symbol.type = factor.type;
            gen(plusMinus.op,"0",factor.place,symbol.place);
            symbolStack.push_back(symbol);
        }},
        //条件控制表达式
        {B,{B,OR,N,BORTERM},[&]() {
            int repeat = 4;
            while (repeat--) {
                stateStack.pop();
            }
            auto bOrTerm = symbolStack.back();
            symbolStack.pop_back();
            auto n = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto b = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(B);
            backpatch(b.falseList,n.quad);
            symbol.trueList = merge(b.trueList,bOrTerm.trueList);
            symbol.falseList = bOrTerm.falseList;
            symbolStack.push_back(symbol);
        }},
        {B,{BORTERM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto bOrTerm = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(B);
            symbol.trueList = bOrTerm.trueList;
            symbol.falseList = bOrTerm.falseList;
            symbolStack.push_back(symbol);
        }},
        {BORTERM,{BORTERM,AND,N,BANDTERM},[&]() {
            int repeat = 4;
            while (repeat--) {
                stateStack.pop();
            }
            auto bAndTerm = symbolStack.back();
            symbolStack.pop_back();
            auto n = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto bOrTerm = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(BORTERM);
            backpatch(bOrTerm.trueList,n.quad);
            symbol.falseList = merge(bOrTerm.falseList,bAndTerm.falseList);
            symbol.trueList = bAndTerm.trueList;
            symbolStack.push_back(symbol);
        }},
        {BORTERM,{BANDTERM},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            Symbol symbol(BORTERM);
            auto bAndTerm = symbolStack.back();
            symbolStack.pop_back();
            symbol.trueList = bAndTerm.trueList;
            symbol.falseList = bAndTerm.falseList;
            symbolStack.push_back(symbol);
        }},
        {BANDTERM,{LBRACE1,B,RBRACE1},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            symbolStack.pop_back();
            auto b = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(BANDTERM);
            symbol.trueList = b.trueList;
            symbol.falseList = b.falseList;
            symbolStack.push_back(symbol);
        }},
        {BANDTERM,{NOT,BANDTERM},[&]() {
            int repeat = 2;
            while (repeat--) {
                stateStack.pop();
            }
            auto bAndTerm = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            Symbol symbol(BANDTERM);
            symbol.trueList = bAndTerm.falseList;
            symbol.falseList = bAndTerm.trueList;
            symbolStack.push_back(symbol);
        }},
        {BANDTERM,{BFACTOR,REL,BFACTOR},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto bFactor2 = symbolStack.back();
            symbolStack.pop_back();
            auto rel = symbolStack.back();
            symbolStack.pop_back();
            auto bFactor1 = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(BANDTERM);
            symbol.trueList = mklist(nxq);
            symbol.falseList = mklist(nxq + 1);
            gen("j" + rel.op,bFactor1.place,bFactor2.place,"0");
            gen("j","-","-","0");
            symbolStack.push_back(symbol);
        }},
        {BANDTERM,{BFACTOR},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto bFactor = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(BANDTERM);
            symbol.trueList = mklist(nxq);
            symbol.falseList = mklist(nxq + 1);
            gen("jnz",bFactor.place,"-","0");
            gen("j","-","-","0");
            symbolStack.push_back(symbol);
        }},
        {BFACTOR,{UINT},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(BFACTOR);
            symbol.place = newTemp(SYMINT);
            symbol.type = SYMINT;
            gen("=",numAndId.front(),"-",symbol.place);
            numAndId.pop_front();
            symbolStack.push_back(symbol);
        }},
        {BFACTOR,{UDOUBLE},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(BFACTOR);
            symbol.place = newTemp(SYMDOUBLE);
            symbol.type = SYMDOUBLE;
            gen("=",numAndId.front(),"-",symbol.place);
            numAndId.pop_front();
            symbolStack.push_back(symbol);
        }},
        {BFACTOR,{ID},[&]() {
            int repeat = 1;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            Symbol symbol(BFACTOR);
            symbol.place = lookup(id.name);
            symbol.type = lookup_type(id.name);
            symbolStack.push_back(symbol);
        }},
        //运算符
        {PLUS_MINUS,{PLUS},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(PLUS_MINUS);
            symbol.op = "+";
            symbolStack.push_back(symbol);
        }},
        {PLUS_MINUS,{MINUS},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(PLUS_MINUS);
            symbol.op = "-";
            symbolStack.push_back(symbol);
        }},
        {MUL_DIV,{TIMES},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(MUL_DIV);
            symbol.op = "*";
            symbolStack.push_back(symbol);
        }},
        {MUL_DIV,{DIVISION},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(MUL_DIV);
            symbol.op = "/";
            symbolStack.push_back(symbol);
        }},
        {REL,{EQUEQU},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(REL);
            symbol.op = "==";
            symbolStack.push_back(symbol);
        }},
        {REL,{NOTEQU},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(REL);
            symbol.op = "!=";
            symbolStack.push_back(symbol);
        }},
        {REL,{LESS},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(REL);
            symbol.op = "<";
            symbolStack.push_back(symbol);
        }},
        {REL,{LE},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(REL);
            symbol.op = "<=";
            symbolStack.push_back(symbol);
        }},
        {REL,{GREATER},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(REL);
            symbol.op = ">";
            symbolStack.push_back(symbol);
        }},
        {REL,{GE},[&]() {
            int repeat = 1;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(REL);
            symbol.op = ">=";
            symbolStack.push_back(symbol);
        }},
        //读写语句
        {SCANF,{SCANF_BEGIN,RBRACE1},[&]() {
            int repeat = 2;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(SCANF);
            symbolStack.push_back(symbol);
        }},
        {SCANF_BEGIN,{SCANF_BEGIN,COMMA,ID},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto p = lookup(id.name);
            gen("R","-","-",p);
            Symbol symbol(SCANF_BEGIN);
            symbolStack.push_back(symbol);
        }},
        {SCANF_BEGIN,{SCANFSYM,LBRACE1,ID},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto p = lookup(id.name);
            gen("R","-","-",p);
            Symbol symbol(SCANF_BEGIN);
            symbolStack.push_back(symbol);
        }},
        {PRINTF,{PRINTF_BEGIN,RBRACE1},[&]() {
            int repeat = 2;
            while (repeat--) {
                symbolStack.pop_back();
                stateStack.pop();
            }
            Symbol symbol(PRINTF);
            symbolStack.push_back(symbol);
        }},
        {PRINTF_BEGIN,{PRINTFSYM,LBRACE1,ID},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto p = lookup(id.name);
            gen("W","-","-",p);
            Symbol symbol(PRINTF_BEGIN);
            symbolStack.push_back(symbol);
        }},
        {PRINTF_BEGIN,{PRINTF_BEGIN,COMMA,ID},[&]() {
            int repeat = 3;
            while (repeat--) {
                stateStack.pop();
            }
            auto id = symbolStack.back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            auto p = lookup(id.name);
            gen("W","-","-",p);
            Symbol symbol(PRINTF_BEGIN);
            symbolStack.push_back(symbol);
        }},
    };
    for (int i = 0; i < grammar.size(); i++) {
        grammarIndex[grammar[i].left].push_back(i);
    }

    //获取first
    for (auto type : allTypes) {
        if (endsymbolsSet.find(type) != endsymbolsSet.end()) {
            //如果是终结符号，那么first就是它本身
            first[type].insert(type);
        }
        else {
            first[type] = set<Type>();
        }
    }
    while (1) {
        bool isChanged = false;
        for (auto& expression : grammar) {
            //获取文法右侧的first值
            set<Type> result = getFirstForCandidate(expression.right);
            if (first[expression.left].size() == 0 && result.size() != 0)
            {
                //为空，就把result加入进来，同时将记录发生改变的isChanged变量置为true
                first[expression.left] = result;
                isChanged = true;
            }
            else
            {
                //不为空，就插入右侧的first，如果有改变就置isChanged为true
                for (auto& type : result)
                {
                    auto res = first[expression.left].insert(type);
                    if (res.second) {
                        isChanged = true;
                    }
                }
            }
        }
        if (!isChanged) {
            //如果不发生改变了就直接break
            break;
        }
    }

    //获取follow
    //首先初始化
    for (auto type : allTypes) {
        if (endsymbolsSet.find(type) == endsymbolsSet.end()) {
            follow[type] = set<Type>();
        }
    }
    follow[START] = { END };
    for (auto& expression : grammar) {
        int k = expression.right.size() - 1;
        for (int i = 0; i < k - 1; i++) {
            if (endsymbolsSet.find(expression.right[i]) == endsymbolsSet.end()) {
                //产生式右侧的紧挨着的符号的first为前一个的follow
                set<Type> result = getFirstForCandidate(vector<Type>(expression.right.begin() + i + 1, expression.right.end()));
                result.erase(ONE);
                if (result.size() != 0) {
                    follow[expression.right[i]].insert(result.begin(), result.end());
                }
            }
        }
    }
    bool isChanged = true;
    while (isChanged) {
        isChanged = false;
        for (auto& expression : grammar) {
            int k = expression.right.size() - 1;
            if (endsymbolsSet.find(expression.right[k]) == endsymbolsSet.end()) {
                //如果产生式右侧的最后一个为非终结符，则最后一个符号的follow加入到产生式左侧的follow集合中
                for (auto& type : follow[expression.left]) {
                    auto res = follow[expression.right[k]].insert(type);
                    if (res.second) {
                        isChanged = true;
                    }
                }
            }
            else {
                continue;
            }
            for (int i = k - 1; i >= 0; i--) {
                //在产生式的右侧的所有符号从后往前来看，如果存在first集合包含空，则往前的符号的follow也加入到产生式左侧集合中。
                if (endsymbolsSet.find(expression.right[i]) == endsymbolsSet.end() && first[expression.right[i + 1]].find(ONE) != first[expression.right[i + 1]].end()) {
                    for (auto& type : follow[expression.left]) {
                        auto res = follow[expression.right[i]].insert(type);
                        if (res.second) {
                            isChanged = true;
                        }
                    }
                }
                else {
                    break;
                }
            }
        }
    }
    //getC
    //初始化项目集闭包
    int state = 0;
    //首先是处理拓广文法的S'->S
    ClosureItem startItem = { (grammarIndex[START].begin())[0],0,{END} };
    I I0 = Clousre(startItem);
    C.insert({ I0,state });
    int2I[state] = C.begin()->first;
    state++;
    stateStack.push(C.begin()->second);
    list<set<pair<I, int>>::iterator> closureQueue;
    closureQueue.push_back(C.begin());
    while (!closureQueue.empty()) {
        auto cur = closureQueue.front();
        auto i = cur->first;
        auto curState = cur->second;
        closureQueue.pop_front();
        for (auto& item : i) {
            //对于状态i的每一个表达式item
            if (item.dot < grammar[item.expressionIndex].right.size()) {
                //获取下一个符号a
                Type a = grammar[item.expressionIndex].right[item.dot];
                I newI = Go(i, a);      //移进a得到新的状态newI
                auto res = C.insert({ newI,state });
                if (res.second) {//如果不存在，就直接插入新状态
                    StateTransSet.insert({ curState,a,state });
                    //cout<<"StateTrans "<<curState<<" "<<a<<" "<<state<<endl;
                    int2I[state] = res.first->first;
                    state++;
                    closureQueue.push_back(res.first);
                }
                else {//项目集中已经存在
                    auto temp = *res.first;
                    auto tempI = temp.first;
                    bool outChanged = false;
                    for (auto& closureIt : newI) {//对于新的I中的每一个项目
                        auto res2 = tempI.find(closureIt);//在已有的I中查找是否有相同的项目
                        if (res2 != tempI.end()) {//如果有
                            bool changed = false;
                            auto tempItem = *res2;
                            for (auto& type : closureIt.lookAhead) {
                                auto res3 = tempItem.lookAhead.insert(type);
                                if (res3.second) {
                                    changed = true;
                                    outChanged = true;
                                }
                            }
                            if (changed) {
                                tempI.erase(res2);
                                tempI.insert(tempItem);
                            }
                        }
                    }
                    temp.first = tempI;
                    if (outChanged) {
                        C.erase(res.first);
                        auto delIt = find(closureQueue.begin(), closureQueue.end(), res.first);
                        if (delIt != closureQueue.end()) {
                            closureQueue.erase(delIt);
                        }
                        C.insert(temp);
                        closureQueue.push_back(C.find(temp));
                    }
                    StateTransSet.insert({ curState,a,temp.second });
                }
            }
        }
    }
    //getTable
    for (auto& trans : StateTransSet) {
        if (isEndSymbol(trans.go)) {
            // 终结符：直接移进
            analyticalTable[trans.oldState][trans.go] = [&]() {
                Symbol symbol(input.front());
                input.pop_front();
                symbolStack.push_back(symbol);
                stateStack.push(trans.newState);
                };
        }
        else {
            // 非终结符进行goto转移，将更新状态栈
            analyticalTable[trans.oldState][trans.go] = [&]() {
                stateStack.push(trans.newState);
                };
        }
    }
    for (auto i = C.begin(); i != C.end(); i++) {
        for (auto& item : i->first) {
            if (item.dot == grammar[item.expressionIndex].right.size()) {
                if (grammar[item.expressionIndex].left == START) {
                    analyticalTable[i->second][END] = [&]() {
                        input.pop_front();
                        };
                }
                else {//归约
                    for (auto& type : item.lookAhead) {
                        analyticalTable[i->second][type] = [&]() {

                            grammar[item.expressionIndex].action();

                            auto action = analyticalTable[stateStack.top()][grammar[item.expressionIndex].left];
                            if (action) {
                                action();
                            }
                            else {
                                abort();
                            }
                            };
                    }
                }
            }
        }
    }
}

void Parser::run()
{
    while (!input.empty())
    {
        auto state = stateStack.top();
        auto symbol = input.front();
        auto action = analyticalTable[state][symbol];
        if (action)
        {
            action();
        }
        else if (symbol != ONE) {
            symbol = ONE;
            action = analyticalTable[state][symbol];
            if (action) {
                action();
            }
            else {
                cout << "Syntax Error" << endl;
                exit(0);
                break;
            }
        }
        else
        {
            cout << "Syntax Error" << endl;
            exit(0);
            break;
        }
    }
}
void Parser::output()
{
    cout << symbolTable.size() << endl;
    for (auto& item : symbolTable) {
        cout << item.name << " " << item.type << " " << item.value << " " << item.offset << endl;
    }
    cout << tempIndex << endl;
    cout << QuardrupleTable.size() << endl;
    for (size_t i = 0; i < QuardrupleTable.size(); i++) {
        cout << i << ": (" << QuardrupleTable[i].op << "," << QuardrupleTable[i].arg1 << "," << QuardrupleTable[i].arg2 << "," << QuardrupleTable[i].result << ")" << endl;
    }
}

int main() {
    getinpu();
    preProcess();
    lex();
    Parser parser;
    parser.run();
    parser.output();
    return 0;
}
