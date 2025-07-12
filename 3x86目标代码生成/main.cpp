#include<bits/stdc++.h>
using namespace std;

// 四元式项结构体（操作符、操作数或结果）
struct QuadrupleItem {
    string val;        // 值
    int use = -1;      // 待用信息（下次使用的索引）
    bool live = false; // 活跃信息（是否活跃）
    QuadrupleItem(string val) :val(val) {}
    QuadrupleItem() {}
};

// 四元式结构
struct Quardruple {
    QuadrupleItem op;    // 操作符
    QuadrupleItem opnd1; // 左操作数
    QuadrupleItem opnd2; // 右操作数
    QuadrupleItem left;  // 结果（左值）
};

// 符号表项
struct symbolTableItem {
    string name = "";    // 变量名
    string type = "";    // 变量类型
    string value = "";   // 变量值
    int offset = -1;     // 栈帧偏移量
    int use = -1;       // 待用信息
    bool isTemp = true;  // 是否是临时变量
    bool live = false;   // 活跃信息
    symbolTableItem() {}
    symbolTableItem(bool isTemp) :isTemp(isTemp) {}
};

// 操作符到汇编指令的映射
map<string, string> opMap = {
    {"+","add"},  // 加法
    {"-","sub"},  // 减法
    {"*","mul"},  // 乘法
    {"/","div"},  // 除法
    {"=","mov"},  // 赋值
    {"<","cmp"},  // 比较（小于）
    {">","cmp"},  // 比较（大于）
    {"<=","cmp"}, // 比较（小于等于）
    {">=","cmp"}, // 比较（大于等于）
    {"==","cmp"}, // 比较（等于）
    {"!=","cmp"}, // 比较（不等于）
    {"&&","and"}, // 逻辑与
    {"||","or"},  // 逻辑或
    {"!","not"},  // 逻辑非
};

// 条件跳转指令映射
map<string, string> jThetaMap = {
    {"j<","jl"},   // 小于跳转
    {"j>","jg"},   // 大于跳转
    {"j<=","jle"}, // 小于等于跳转
    {"j>=","jge"}, // 大于等于跳转
    {"j==","je"},  // 等于跳转
    {"j!=","jne"}, // 不等于跳转
};

// 条件设置指令映射（用于比较结果）
map<string, string> cmpMap = {
    {"<","setl"},  // 设置小于
    {">","setg"},  // 设置大于
    {"<=","setle"},// 设置小于等于
    {">=","setge"},// 设置大于等于
    {"==","sete"}, // 设置等于
    {"!=","setne"},// 设置不等于
};

// 判断是否为二元操作（θ运算）
bool isTheta(Quardruple& q) {
    // 操作符是二元运算符且右操作数不为空
    return (q.op.val[0] == '+' || q.op.val[0] == '-' || q.op.val[0] == '*' || 
            q.op.val[0] == '/' || q.op.val[0] == '=' || q.op.val[0] == '<' || 
            q.op.val[0] == '>' || q.op.val[0] == '!' || q.op.val[0] == '&' || 
            q.op.val[0] == '|') && (q.opnd2.val != "-");
}

// 判断是否为条件跳转（jθ）
bool isJTheta(Quardruple& q) {
    return q.op.val[0] == 'j' && q.op.val != "j" && q.op.val != "jnz";
}

// 判断是否为无条件跳转（j）
bool isJ(Quardruple& q) {
    return q.op.val == "j";
}

// 判断是否为非零跳转（jnz）
bool isJnz(Quardruple& q) {
    return q.op.val == "jnz";
}

// 判断是否为读/写操作（R/W）
bool isROrW(Quardruple& q) {
    return q.op.val == "W" || q.op.val == "R";
}

// 判断是否为单操作数操作（如赋值或取反）
bool isOnlyX(Quardruple& q) {
    return (q.op.val == "=" || q.op.val == "!") && q.opnd2.val == "-";
}

// 判断是否为结束指令
bool isEnd(Quardruple& q) {
    return q.op.val == "End";
}

// 判断是否为数字常量
bool isUNum(string& s) {
    return s[0] >= '0' && s[0] <= '9';
}

// 地址描述符项（记录变量存储位置）
struct AvalItem {
    set<string>  reg; // 存储的寄存器
    set<string>  mem; // 存储的内存位置
};

// 全局变量
int initialOffset = 0;    // 初始栈帧偏移量
int offset = 0;        // 当前栈帧偏移量
vector<Quardruple> quadruples; // 四元式序列
map<string, symbolTableItem> symbolTable; // 符号表
vector<vector<int> > basicBlocks; // 基本块列表
int tempVarCount = 0;   // 临时变量计数
map<string, AvalItem> Aval; // 地址描述符表
map<string, set<string> > Rval; // 寄存器描述符表
const vector<string> regs = { "R0","R1","R2" }; // 可用寄存器
vector<set<string> > liveOut; // 各基本块出口活跃变量集
map<string, QuadrupleItem> hInfo; // 历史活跃信息
vector<int> labelFlag; // 标签标记（1表示需要生成标签）

// 根据四元式索引查找所在基本块
int findBlockIndex(int index) {
    for (int i = 0; i < basicBlocks.size(); i++) {
        if (find(basicBlocks[i].begin(), basicBlocks[i].end(), index) != basicBlocks[i].end()) {
            return i;
        }
    }
    return -1;
}

// 标记需要生成标签的基本块入口
void genLable(int quad) {
    if (labelFlag[quad] == 0) {
        labelFlag[quad] = 1;
    }
}

// 从输入读取符号表
void getInputForSymbolTable(int lenth) {
    string line;
    for (int i = 0; i < lenth; i++) {
        getline(cin, line);
        stringstream ss(line);
        symbolTableItem temp(false);
        ss >> temp.name >> temp.type >> temp.value >> temp.offset;
        initialOffset = max(initialOffset, temp.offset); // 更新最大偏移量
        symbolTable["TB" + to_string(i)] = temp; // 存入符号表
    }
}

// 从输入读取四元式序列
void getInputForquadruples(int lenth) {
    quadruples.resize(lenth);
    string line;
    for (int i = 0; i < lenth; i++) {
        getline(cin, line);
        // 解析四元式格式："(op, opnd1, opnd2, left)"
        line = line.substr(line.find(':') + 2); // 移除行号
        line = line.substr(1, line.find(')') - 1); // 移除括号
        stringstream ss(line);
        vector<string> tokens(4);
        int j = 0;
        // 用逗号分割四个部分
        while (getline(ss, tokens[j], ',')) {
            j++;
        }
        // 存入四元式结构
        quadruples[i].op = tokens[0];
        quadruples[i].opnd1 = tokens[1];
        quadruples[i].opnd2 = tokens[2];
        quadruples[i].left = tokens[3];
    }
}

// 主输入函数
void input() {
    string line;
    getline(cin, line);
    if (line == "Syntax Error") { // 语法错误处理
        cout << "halt" << endl;
        exit(0);
    }
    int symbolTableSize, quadruplesSize;
    symbolTableSize = stoi(line); // 符号表大小
    getInputForSymbolTable(symbolTableSize); // 读取符号表
    getline(cin, line);
    tempVarCount = stoi(line); // 临时变量数量
    getline(cin, line);
    quadruplesSize = stoi(line); // 四元式数量
    getInputForquadruples(quadruplesSize); // 读取四元式
    offset = initialOffset; // 初始化当前偏移量
}

// 计算基本块的活跃变量信息和待用信息
set<string> getUseInfo(vector<int>& block) {
    set<string> res; // 出口活跃变量集合
    
    // 初始化：所有变量在入口处不活跃
    for (auto& i : block) {
        string x = quadruples[i].opnd1.val;
        string y = quadruples[i].opnd2.val;
        string z = quadruples[i].left.val;
        
        // 非临时变量在出口处活跃
        if (x[0] == 'T' && !symbolTable[x].isTemp) {
            symbolTable[x].live = true;
            res.insert(x);
        }
        if (y[0] == 'T' && !symbolTable[y].isTemp) {
            symbolTable[y].live = true;
            res.insert(y);
        }
        if (z[0] == 'T' && !symbolTable[z].isTemp) {
            symbolTable[z].live = true;
            res.insert(z);
        }
    }
    
    // 反向扫描计算待用信息和活跃信息
    for (int i = block.size() - 1; i >= 0; i--) {
        int index = block[i];
        
        // 处理左值（结果）
        if (quadruples[index].left.val[0] == 'T') {
            string var = quadruples[index].left.val;
            quadruples[index].left.use = symbolTable[var].use;
            quadruples[index].left.live = symbolTable[var].live;
            symbolTable[var].live = false; // 定义前不活跃
            symbolTable[var].use = -1;    // 清除待用信息
        }
        
        // 处理左操作数
        if (quadruples[index].opnd1.val[0] == 'T') {
            string var = quadruples[index].opnd1.val;
            quadruples[index].opnd1.use = symbolTable[var].use;
            quadruples[index].opnd1.live = symbolTable[var].live;
            symbolTable[var].live = true;  // 使用前活跃
            symbolTable[var].use = index;  // 设置待用位置
        }
        
        // 处理右操作数
        if (quadruples[index].opnd2.val[0] == 'T') {
            string var = quadruples[index].opnd2.val;
            quadruples[index].opnd2.use = symbolTable[var].use;
            quadruples[index].opnd2.live = symbolTable[var].live;
            symbolTable[var].live = true;  // 使用前活跃
            symbolTable[var].use = index;  // 设置待用位置
        }
    }
    return res; // 返回出口活跃变量集合
}

// 获取变量在栈帧中的地址（必要时分配空间）
string getAddress(string var) {
    if (var[0] == '[') return var; // 已经是地址格式
    // 已分配偏移量的变量
    if (symbolTable.find(var) != symbolTable.end() && symbolTable[var].offset != -1) {
        return "[ebp-" + to_string(symbolTable[var].offset) + "]";
    }
    // 为临时变量分配空间
    if (var.back() == 'i') { // 整型变量
        offset += 4;
        symbolTable[var].offset = offset;
    }
    else if (var.back() == 'd') { // 双精度变量
        offset += 8;
        symbolTable[var].offset = offset;
    }
    return "[ebp-" + to_string(symbolTable[var].offset) + "]";
}

// 选择寄存器策略：找到使用位置最远的寄存器
string findR(vector<string>& allR, int index) {
    string res;
    int maxUse = INT32_MIN;
    int blockIndex = findBlockIndex(index);
    
    for (auto& Ri : allR) {
        bool hasFound = false;
        // 在当前基本块中查找寄存器中变量的下次使用位置
        for (int i = index + 1; i <= basicBlocks[blockIndex].back(); i++) {
            if (Rval[Ri].find(quadruples[i].opnd1.val) != Rval[Ri].end() ||
                Rval[Ri].find(quadruples[i].opnd2.val) != Rval[Ri].end()) {
                hasFound = true;
                if (i > maxUse) {
                    maxUse = i;
                    res = Ri;
                }
                break;
            }
        }
        // 当前寄存器中的变量在基本块中不再使用
        if (!hasFound) {
            res = Ri;
            break;
        }
    }
    return res;
}

// 获取可用寄存器（可能溢出当前内容）
string getReg(int index) {
    Quardruple q = quadruples[index];
    string z = q.left.val; // 结果变量
    string x = q.opnd1.val; // 左操作数
    string y = q.opnd2.val; // 右操作数
    
    // 如果左操作数已在寄存器且满足条件
    if (!isUNum(x) && x != "-") {
        for (auto& Ri : Aval[x].reg) {
            // 寄存器只包含x 且 (x是结果或x不活跃)
            if (Rval[Ri] == set<string>{x} && (x == z || !q.opnd1.live)) {
                return Ri;
            }
        }
    }
    
    // 查找空闲寄存器
    for (auto& Ri : regs) {
        if (Rval[Ri].empty()) return Ri;
    }
    
    // 无空闲寄存器，选择溢出
    vector<string> RA;
    for (auto& Ri : regs) {
        if (!Rval[Ri].empty()) RA.push_back(Ri);
    }
    if (RA.empty()) RA = regs;
    
    string Ri;
    bool hasFound = true;
    // 查找所有变量已有内存副本的寄存器
    for (auto& Rj : RA) {
        hasFound = true;
        for (auto& a : Rval[Rj]) {
            if (Aval[a].mem.find(a) == Aval[a].mem.end()) {
                hasFound = false;
                break;
            }
        }
        if (hasFound) {
            Ri = Rj;
            break;
        }
    }
    
    // 使用策略选择寄存器
    if (!hasFound) {
        Ri = findR(RA, index);
    }
    
    // 溢出当前寄存器内容
    for (auto& a : Rval[Ri]) {
        // 如果变量在内存无副本且不是结果变量
        if (Aval[a].mem.find(a) == Aval[a].mem.end() && a != z) {
            cout << "mov " << getAddress(a) << ", " << Ri << endl; // 生成保存代码
        }
        // 更新地址描述符
        if (a == x || (a == y && Rval[Ri].find(x) != Rval[Ri].end())) {
            Aval[a].mem = { a };
            Aval[a].reg = { Ri };
        }
        else {
            Aval[a].mem = { a };
            Aval[a].reg = {};
        }
    }
    Rval[Ri].clear(); // 清空寄存器
    return Ri; // 返回选择的寄存器
}

// 释放寄存器的占用（如果变量在出口不活跃）
void releaseReg(string var, set<string>& liveOut) {
    if (liveOut.find(var) == liveOut.end()) { // 非出口活跃
        for (auto& reg : Aval[var].reg) {
            Rval[reg].erase(var); // 从寄存器描述符中移除
        }
        Aval[var].reg.clear();   // 清除地址描述符
    }
}

// 生成单操作数指令的目标代码
void genForOnlyX(int index, int blockIndex) {
    Quardruple q = quadruples[index];
    auto x = q.opnd1.val;
    auto z = q.left.val;
    auto R = getReg(index); // 获取寄存器
    
    if (isUNum(x)) { // 常量加载
        cout << "mov " << R << ", " << x << endl;
    }
    else {
        string x1;
        // 如果寄存器中没有x
        if (Rval[R].find(x) == Rval[R].end()) {
            // 查找x的当前位置
            if (!Aval[x].reg.empty()) {
                x1 = *Aval[x].reg.begin();
            }
            else {
                x1 = getAddress(x);
            }
            cout << "mov " << R << ", " << x1 << endl; // 加载x
        }
        if (q.op.val != "=") { // 非赋值操作
            cout << opMap[q.op.val] << " " << R << endl;
        }
        releaseReg(x, liveOut[blockIndex]); // 释放x的寄存器
    }
    
    // 更新结果z的信息
    Rval[R].insert(z);
    hInfo[z] = q.left;
    Aval[z].reg.insert(R);
    Aval[z].mem.clear(); // 内存副本无效
}

// 生成二元运算指令的目标代码
void genForTheta(int index, int blockIndex) {
    Quardruple q = quadruples[index];
    auto x = q.opnd1.val;
    auto y = q.opnd2.val;
    auto z = q.left.val;
    auto Rz = getReg(index); // 获取结果寄存器
    
    // 获取操作数的当前位置
    string x1 = (x != "-" && !isUNum(x)) ? 
                (!Aval[x].reg.empty() ? *Aval[x].reg.begin() : getAddress(x)) 
                : x;
                
    string y1 = (y != "-" && !isUNum(y)) ? 
                (!Aval[y].reg.empty() ? *Aval[y].reg.begin() : getAddress(y)) 
                : y;
    
    // 生成指令序列
    if (x1 == Rz) { // 左操作数在结果寄存器
        cout << opMap[q.op.val] << " " << Rz << ", " << y1 << endl;
        if (opMap[q.op.val] == "cmp") { // 比较操作额外生成设置
            cout << cmpMap[q.op.val] << " " << Rz << endl;
        }
        Aval[x].reg.erase(Rz); // 更新x的地址描述符
    }
    else {
        cout << "mov " << Rz << ", " << x1 << endl; // 加载左操作数
        cout << opMap[q.op.val] << " " << Rz << ", " << y1 << endl;
        if (opMap[q.op.val] == "cmp") {
            cout << cmpMap[q.op.val] << " " << Rz << endl;
        }
    }
    
    // 更新操作数和结果的信息
    if (y1 == Rz && !isUNum(y)) {
        Aval[y].reg.erase(Rz);
    }
    Rval[Rz] = { z };
    hInfo[z] = q.left;
    Aval[z].reg = { Rz };
    Aval[z].mem.clear();
    
    // 释放操作数寄存器
    if (!isUNum(x)) releaseReg(x, liveOut[blockIndex]);
    if (!isUNum(y)) releaseReg(y, liveOut[blockIndex]);
}

// 生成读/写操作的目标代码
void genForRorW(Quardruple& q, int blockIndex) {
    if (q.op.val == "W") {
        cout << "jmp ?write"; // 写操作
    }
    else {
        cout << "jmp ?read"; // 读操作
    }
    cout << "(" << getAddress(q.left.val) << ")" << endl;
    releaseReg(q.left.val, liveOut[blockIndex]); // 释放操作数
}

// 生成结束指令
void genForEnd(Quardruple& q) {
    cout << "halt" << endl;
}

// 清空符号表（未实际使用）
void clearSymbolTable() {
    offset = initialOffset;
    for (auto& a : symbolTable) {
        if (a.second.isTemp) {
            a.second.use = -1;
            a.second.live = false;
            a.second.offset = -1;
        }
    }
}

// 生成目标代码的主函数
void genCode() {
    // 遍历所有基本块
    for (int blockIndex = 0; blockIndex < basicBlocks.size(); blockIndex++) {
        auto& block = basicBlocks[blockIndex];
        
        // 基本块标签生成
        if (labelFlag[basicBlocks[blockIndex].front()] == 1) {
            cout << "?" + to_string(block.front()) + ":" << endl;
        }
        
        // 遍历基本块内语句
        for (auto j : block) {
            if (isTheta(quadruples[j])) { // 二元操作
                genForTheta(j, blockIndex);
            }
            else if (isOnlyX(quadruples[j])) { // 一元操作
                genForOnlyX(j, blockIndex);
            }
            else if (isROrW(quadruples[j])) { // 读/写操作
                genForRorW(quadruples[j], blockIndex);
            }
        }
        
        // 基本块结束前：保存需要写回的变量
        for (auto a : liveOut[blockIndex]) {
            // 如果变量在内存中没有副本
            if (Aval[a].mem.find(a) == Aval[a].mem.end()) {
                if (!Aval[a].reg.empty()) {
                    for (auto reg : Aval[a].reg) {
                        if (hInfo[a].live) { // 活跃变量写回
                            cout << "mov " << getAddress(a) << ", " << reg << endl;
                        }
                    }
                }
            }
        }
        
        // 处理基本块结尾的跳转
        auto qini = quadruples[block.back()];
        if (isJ(qini)) { // 无条件跳转
            cout << "jmp ?" << qini.left.val << endl;
        }
        else if (isJTheta(qini)) { // 条件跳转
            auto x = qini.opnd1.val;
            auto y = qini.opnd2.val;
            auto q = qini.left.val;
            
            // 获取操作数的位置
            string x1 = !Aval[x].reg.empty() ? *Aval[x].reg.begin() : x;
            string y1 = !Aval[y].reg.empty() ? *Aval[y].reg.begin() : getAddress(y);
            
            if (x1 == x) { // 需要加载到寄存器
                x1 = getReg(block.back());
                cout << "mov " << x1 << ", " << getAddress(x) << endl;
            }
            cout << "cmp " << x1 << ", " << y1 << endl; // 比较
            cout << jThetaMap[qini.op.val] << " ?" << q << endl; // 条件跳转
        }
        else if (isJnz(qini)) { // 非零跳转
            auto x = qini.opnd1.val;
            auto q = qini.left.val;
            
            string x1 = !Aval[x].reg.empty() ? *Aval[x].reg.begin() : x;
            if (x1 == x) {
                x1 = getReg(block.back());
                cout << "mov " << x1 << ", " << getAddress(x) << endl;
            }
            cout << "cmp " << x1 << ", 0" << endl;
            cout << "jne" << " ?" << q << endl;
        }
        else if (isEnd(qini)) { // 结束
            genForEnd(qini);
        }
        
        // 清空描述符准备下一个基本块
        Rval.clear();
        Aval.clear();
    }
}
// 基本块划分
void getBasicBlock() {
    set<vector<int> > blocks; // 基本块集合
    int isEnter[quadruples.size()] = { false }; // 入口标记数组
    
    // Step 1: 标记所有入口语句
    isEnter[0] = true; // 第一条是入口
    for (int i = 0; i < quadruples.size(); i++) {
        if (isJTheta(quadruples[i]) || isJnz(quadruples[i])) { // 条件跳转
            int index = stoi(quadruples[i].left.val); // 跳转目标
            isEnter[index] = true; // 标记跳转目标为入口
            if (i < quadruples.size() - 1) {
                isEnter[i + 1] = true; // 下一语句也是入口
            }
            genLable(index); // 标记需要标签
        }
        else if (isJ(quadruples[i])) { // 无条件跳转
            int index = stoi(quadruples[i].left.val);
            isEnter[index] = true;
            genLable(index);
        }
        else if (quadruples[i].op.val == "End") { // 结束语句
            isEnter[quadruples.size() - 1] = true;
        }
        else if (isROrW(quadruples[i])) { // 读写操作单独成块
            isEnter[i] = true;
        }
    }

    // Step 2: 构建基本块
    int i = 0;
    while (i < quadruples.size()) {
        if (isEnter[i]) {
            // 处理最后一条语句
            if (i == quadruples.size() - 1) {
                vector<int> temp;
                temp.push_back(i);
                blocks.insert(temp);
            }
            if (i + 1 == quadruples.size()) break;
            
            // 寻找当前基本块结束位置
            for (int j = i + 1; j < quadruples.size(); j++) {
                if (isEnter[j]) { // 遇到下一个入口
                    vector<int> temp;
                    for (int k = i; k < j; k++) { // 添加语句
                        temp.push_back(k);
                    }
                    blocks.insert(temp);
                    i = j;
                    break;
                }
                else if (quadruples[j].op.val[0] == 'j' || 
                        quadruples[j].op.val == "ret" || 
                        quadruples[j].op.val == "End") { // 跳转或结束语句
                    vector<int> temp;
                    for (int k = i; k <= j; k++) {
                        temp.push_back(k);
                    }
                    blocks.insert(temp);
                    i = j + 1;
                    break;
                }
            }
        }
        else {
            i++;
        }
    }
    // 将set转为vector
    basicBlocks.assign(blocks.begin(), blocks.end());
}


int main() {
    input();  // 读取输入
    labelFlag.resize(quadruples.size(), 0); // 初始化标签标记
    getBasicBlock(); // 划分基本块
    liveOut.resize(basicBlocks.size());
    for (int i = 0; i < basicBlocks.size(); i++) {
        liveOut[i] = getUseInfo(basicBlocks[i]); // 计算活跃变量信息
    }
    genCode(); // 生成目标代码
    return 0;
}