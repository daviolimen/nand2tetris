#include <bits/stdc++.h>
using namespace std;

enum commandTypes {
    C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL
};

map<string, commandTypes> commandToType = {
    {"push", C_PUSH},
    {"pop", C_POP},
    {"add", C_ARITHMETIC},
    {"sub", C_ARITHMETIC},
    {"neg", C_ARITHMETIC},
    {"eq", C_ARITHMETIC},
    {"gt", C_ARITHMETIC},
    {"lt", C_ARITHMETIC},
    {"and", C_ARITHMETIC},
    {"or", C_ARITHMETIC},
    {"not", C_ARITHMETIC},
    {"label", C_LABEL},
    {"goto", C_GOTO},
    {"if-goto", C_IF},
    {"function", C_FUNCTION},
    {"return", C_RETURN},
    {"call", C_CALL}
};

class Parser {
    private:
        bool hasMoreLines;
        commandTypes commandType;
        string currentCommand, firstArg;
        int secondArg;

        void trimString(string& s) {
            int start = s.find_first_not_of(" \t\n\r\f\v");
            s = (start == string::npos) ? "" : s.substr(start);
            int end = s.find_last_not_of(" \t\n\r\f\v");
            s = (end == string::npos) ? "" : s.substr(0, end + 1);
        }


    public:
        Parser(const string& inputFile) {
            if (!freopen(inputFile.c_str(), "r", stdin)) {
                cout << "File specified could not be found.\n";
                return;
            }
            hasMoreLines = 1;
        }
    
        void advanceLine() {
            bool ok = 0;
            while (getline(cin, currentCommand)) {
                ok = 1;
                currentCommand = currentCommand.substr(0, currentCommand.find("//"));
                trimString(currentCommand);
                if (currentCommand.empty()) continue;
                break;
            }
            if (!ok) {
                hasMoreLines = 0;
                return;
            }
            parseCommand();
        }

        void parseCommand() {
            istringstream iss(currentCommand);
            vector<string> tokens;
            string token;
            while (iss >> token) {
                tokens.push_back(token);
            }

            commandType = commandToType[tokens[0]];
            if (commandType == C_RETURN) {
                firstArg = "";
                secondArg = -1;
                return;
            }
            if (commandType == C_ARITHMETIC) {
                firstArg = tokens[0];
                secondArg = -1;
                return;
            }
            firstArg = tokens[1];
            secondArg = -1;
            if ((commandType == C_PUSH) || (commandType == C_POP) || (commandType == C_FUNCTION) || (commandType == C_CALL)) {
                secondArg = stoi(tokens[2]);
            }
        }

        bool getHasMoreLines() {
            return hasMoreLines;
        }

        commandTypes getCommandType() {
            return commandType;
        }

        string getFirstArg() {
            return firstArg;
        }

        int getSecondArg() {
            return secondArg;
        }
};

class CodeWriter {
    private:
        int labelCnt;
        string name;
    public:
        CodeWriter(const string &fileName) {
            labelCnt = 0;
            name = fileName;
            freopen((name + ".asm").c_str(), "w", stdout);
            cout << "// bootstrap code\n@256\nD=A\n@SP\nM=D\n";
        }
        
        void writeArithmetic(const string &command) {
            string codeToWrite = "// " + command + " command\n";
            if ((command == "neg") || (command == "not")) {
                codeToWrite += "@SP\nA=M-1\n";
                if (command == "neg") codeToWrite += "M=-M\n";
                else codeToWrite += "M=!M\n";
            } else if ((command == "eq") || (command == "gt") || (command == "lt")) {
                codeToWrite += "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\nM=-1\n@__COMPARISON_LABEL_" + to_string(labelCnt) + "\n";
                if (command == "eq") codeToWrite += "D;JEQ\n";
                else if (command == "gt") codeToWrite += "D;JGT\n";
                else if (command == "lt") codeToWrite += "D;JLT\n";
                codeToWrite += "@SP\nA=M-1\nM=0\n(__COMPARISON_LABEL_" + to_string(labelCnt) + ")\n";
                labelCnt++;
            } else {
                codeToWrite += "@SP\nAM=M-1\nD=M\nA=A-1\n";
                if (command == "add") codeToWrite += "D=D+M\n";
                else if (command == "sub") codeToWrite += "D=M-D\n";
                else if (command == "and") codeToWrite += "D=D&M\n";
                else if (command == "or") codeToWrite += "D=D|M\n";
                codeToWrite += "M=D\n";
            }
            cout << codeToWrite;
        }

        void writePushPop(commandTypes command, const string &segment, int index) {
            string comment;
            if (command == C_PUSH) comment = "push";
            else comment = "pop";

            string codeToWrite = "// " + comment + " " + segment + " " + to_string(index) + "\n";
            
            if (command == C_PUSH) {
                if (segment == "constant") codeToWrite += "@" + to_string(index) + "\nD=A\n";
                else if (segment == "local") codeToWrite += "@LCL\nD=M\n@" + to_string(index) + "\nD=D+A\nA=D\nD=M\n";
                else if (segment == "temp") codeToWrite += "@" + to_string(5 + index) + "\nD=M\n";
                else if (segment == "this") codeToWrite += "@THIS\nD=M\n@" + to_string(index) + "\nD=D+A\nA=D\nD=M\n";
                else if (segment == "that") codeToWrite += "@THAT\nD=M\n@" + to_string(index) + "\nD=D+A\nA=D\nD=M\n";
                else if (segment == "argument") codeToWrite += "@ARG\nD=M\n@" + to_string(index) + "\nD=D+A\nA=D\nD=M\n";
                else if (segment == "pointer") codeToWrite += "@" + to_string(3 + index) + "\nD=M\n";
                else if (segment == "static") codeToWrite += "@" + name + "." + to_string(index) + "\nD=M\n";
                codeToWrite += "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
            } else {
                if (segment == "local") codeToWrite += "@LCL\nD=M\n@" + to_string(index) + "\nD=D+A\n";
                else if (segment == "temp") codeToWrite += "@" + to_string(5 + index) + "\nD=A\n";
                else if (segment == "this") codeToWrite += "@THIS\nD=M\n@" + to_string(index) + "\nD=D+A\n";
                else if (segment == "that") codeToWrite += "@THAT\nD=M\n@" + to_string(index) + "\nD=D+A\n";
                else if (segment == "argument") codeToWrite += "@ARG\nD=M\n@" + to_string(index) + "\nD=D+A\n";
                else if (segment == "pointer") codeToWrite += "@" + to_string(3 + index) + "\nD=A\n";
                else if (segment == "static") codeToWrite += "@" + name + "." + to_string(index) + "\nD=A\n";
                codeToWrite += "@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
            }
            cout << codeToWrite;
        }

        void writeLabel(const string &label) {
            string codeToWrite = "// label " + label + "\n(" + label + ")\n";
            cout << codeToWrite;
        }

        void writeGoto(const string &label) {
            string codeToWrite = "// goto " + label + "\n@" + label + "\n0;JMP\n";
            cout << codeToWrite;
        }
        
        void writeIf(const string &label) {
            string codeToWrite = "// if-goto" + label + "\n";
            codeToWrite += "@SP\nAM=M-1\nD=M\n@" + label + "\nD;JNE\n";
            cout << codeToWrite;
        }

        void writeFunction(const string &functionName, int nVars) {
            string codeToWrite = "// function " + functionName + " " + to_string(nVars) + "\n";
            codeToWrite += "(" + functionName + ")\n";
            for (int i = 0; i < nVars; i++) codeToWrite += "@0\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
            cout << codeToWrite;
        }

        void writeCall(const string &functionName, int nArgs) {
            string codeToWrite = "// call " + functionName + " " + to_string(nArgs) + "\n";
            
        }

        void writeReturn() {
            
        }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Invalid argument count\n";
        return -1;
    }

    string inputFile(argv[1]);
    string name = inputFile.substr(0, inputFile.size() - 3);

    Parser psr(inputFile);
    CodeWriter cwr(name);

    while (true) {
        psr.advanceLine();
        if (!psr.getHasMoreLines()) break;
        auto commType = psr.getCommandType();

        if (commType == C_ARITHMETIC) cwr.writeArithmetic(psr.getFirstArg());
        else if ((commType == C_PUSH) || (commType == C_POP)) cwr.writePushPop(commType, psr.getFirstArg(), psr.getSecondArg());
        else if (commType == C_LABEL) cwr.writeLabel(psr.getFirstArg());
        else if (commType == C_GOTO) cwr.writeGoto(psr.getFirstArg());
        else if (commType == C_IF) cwr.writeIf(psr.getFirstArg());
        
    }
}