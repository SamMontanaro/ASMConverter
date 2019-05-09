#include <iostream> //For strings
#include <fstream> //For files
#include <string> //For strings
#include <stack> //For stacks
#include <vector> //For dynamic arrays, needed mostly for line numbers
#include <cmath> //For floor, might remove
#include <regex> //For syntax parsing
#include <unordered_map> //For storing labels and values/registers/etc.
#include <functional> //For function objects
#include <ctime>

#include "main.h"

using namespace std;

/*
9:58 PM
Technidecimal
HECCIN DUNNIT
we gucci in the hucci
let's call it a night



we're at 11:44 PM now
*/

int main() {
	vector<vector<string>> dataInstructions;
	vector<vector<string>> codeInstructions;

	{ //Encapsulate these ugly variables; it's so we don't hold on to extra unneeded variables
		ifstream assemblyInput("instructions.asm");
		if (!assemblyInput.is_open()) {
			cerr << "Error opening assembly instructions file. Exiting..." << endl;
			system("PAUSE");
			return 1;
		}

		bool inDataSegment = false;
		bool inCodeSegment = false;
		int commentPos;
		regex expression("(?:\\w+|,|\\+|\\-|\\[|\\]|:|\\?|\\$)");
		sregex_iterator endCondition;
		string line;

		getline(assemblyInput, line); // Pumping and priming
		while (!assemblyInput.fail()) {
			commentPos = (line + ";").find_first_of(";");
			line = trim(line.substr(0, commentPos));

			if (line != "") {
				if (line == ".data") {
					inDataSegment = !(inCodeSegment = false);
				}
				else if (line == ".code") {
					inCodeSegment = !(inDataSegment = false);
				}
				else if (line[0] == '.') {
					inDataSegment = (inCodeSegment = false);
				}
				else if (inDataSegment || inCodeSegment) {
					vector<string> words = regex_matches(line, expression);
					if (words.size() > 0) {
						(inDataSegment ? dataInstructions : codeInstructions).push_back(words);
					}
				}
			}

			if (assemblyInput.eof()) {
				break;
			}
			else {
				getline(assemblyInput, line);
			}
		}
		assemblyInput.close();
	}

	//Okay, now that we've populated the data and code instructions, it's time to start executing them
	//jk we actually have to scan through them once first to save all labels and functions
	//jk again we need to initialize our own shit first

	LabelData AssemblyData;
	vector<string> AssemblyMemory;
	vector<string> myInstructs;
	vector<Label> myLabels;
	int myInstructIterator = 0;
	int myInstructOffset = 0;
	int myLabelIterator = 0; //AHH FINALLY OUR ITERATOR
	bool isPrepping = true; //Only set up procs and labels, things like mov should gtfo
	bool isInCodeSegment;
	{ //Encapsulate temp variables agane
		int i = 0;
		string v;
		while ((v = Label_Keywords[i++]) != "END_OF_KEYWORDS") {
			//AssemblyData[v] = Label(toLowercase(v["name"]), v["handler"]);
		}
	}
	AssemblyData["cout"] = Label("cout", [&]() { //ya ya yotus
		if (isPrepping) {
			return;
		}
		if (myInstructIterator == 0) {
			myInstructs.push_back("cout");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) { // shit i tried lol
			for (int i = 0; i < myLabels.size(); i++) {
				switch (myLabels[i].type) {
				case Label_NULL:
					cout << myLabels[i].name << ' ';
					break;
				case Label_Value: //TODO: format based on size and in binary
				case Label_Register:
					if (myLabels[i].isConstant) {
						cout << (myLabels[i].isInitialized ? to_string(myLabels[i].value.number) : "?") << ' ';
					}
					else {
						cout << (myLabels[i].type == Label_Value ? myLabels[i].name : toUppercase(myLabels[i].name)) << "@" << (myLabels[i].type == Label_Value ? to_string(myLabels[i].offset) : "REGISTER") << '[' << (myLabels[i].isInitialized ? to_string(myLabels[i].value.number) : "?") << "] ";
					}
					break;
				case Label_Offset:
				case Label_Function:
					cout << myLabels[i].name << (myLabels[i].type == Label_Offset ? ":@" : "$PROC$@") << myLabels[i].offset << '[' << myLabels[i].inCodeSegment << (myLabels[i].type == Label_Offset) << "] ";
					break;
				case Label_Keyword:
					cout << '$' << myLabels[i].name << "$ ";
				} // @ denotes offset/address, [] denote additional info like flags or value; alright
			}
			cout << endl;
		}
	}); 
	AssemblyData[":"] = Label(":", [&]() { //for labels like L1:
		if (isPrepping && myInstructIterator == 1 && myLabelIterator == 1 && myLabels[0].type == Label_NULL) {
			myLabels[0] = (AssemblyData[myInstructs[0]] = Label(myInstructs[0], myInstructOffset, isInCodeSegment));
		}
	});
	AssemblyData["proc"] = Label("proc", [&]() { //for PROCS
		if (isPrepping && myInstructIterator == 1 && myLabelIterator == 1 && myLabels[0].type == Label_NULL) {
			myLabels[0] = (AssemblyData[myInstructs[0]] = Label(myInstructs[0], myInstructOffset, isInCodeSegment, false));
		}
	});
	AssemblyData[","] = Label(",", [&]() {
		//myLabelIterator++;
	});
    /*
    Making it a keyword might (keyword might) cause issues in cases like mov ?, 5
    AssemblyData["?"] = Label("?", [&]() {
        if (myLabelIterator + 1 > myLabels.size()) {
            myLabels.resize(myLabelIterator + 1);
        }
        myLabels[myLabelIterator++] = Label("NULL", Value(0, QWORD), true, true, true);;
	});
    */
    AssemblyData["?"] = Label("?", Value(0, QWORD), true, true, true);
    AssemblyData["$"] = Label("$", Value(0, QWORD), true, true, true);
	AssemblyData["mov"] = Label("mov", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("mov");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
			myLabels[0].value = (AssemblyData[myLabels[0].name].value = myLabels[1].value.number).number;
	    }
	});
	AssemblyData["add"] = Label("add", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("add");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value += myLabels[1].value.number).number;
		}
	});
	AssemblyData["sub"] = Label("sub", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("sub");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value -= myLabels[1].value.number).number;
		}
	});
	AssemblyData["inc"] = Label("inc", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("inc");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
			myLabels[0].value = (AssemblyData[myLabels[0].name].value += 1).number;
		}
	});
	AssemblyData["dec"] = Label("dec", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("dec");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
			myLabels[0].value = (AssemblyData[myLabels[0].name].value -= 1).number;
		}
	});
	AssemblyData["dumpregs"] = Label("dumpregs", [&]() {
		if (isPrepping) {
			return;
		}
		//todo: lol this
	});
	AssemblyData["mul"] = Label("mul", [&]() { //todo: fix this shit
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("mul");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value *= myLabels[1].value.number).number;
		}
	});
	AssemblyData["imul"] = Label("imul", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("imul");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value *= myLabels[1].value.number).number;
		}
	});
	AssemblyData["div"] = Label("div", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("div");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value /= myLabels[1].value.number).number;
		}
	});
	AssemblyData["idiv"] = Label("idiv", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("idiv");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value /= myLabels[1].value.number).number;
		}
	});
	AssemblyData["neg"] = Label("neg", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("neg");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
            myLabels[0].value = (AssemblyData[myLabels[0].name].value *= -1).number;
		}
	});
	AssemblyData["xchg"] = Label("xchg", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == 0) {
			myInstructs.push_back("xchg");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1) {
			int temp = myLabels[0].value.number;
			myLabels[0].value = (AssemblyData[myLabels[0].name].value = myLabels[1].value.number).number;
			myLabels[1].value = (AssemblyData[myLabels[1].name].value = temp).number;
		}
	});
	AssemblyData["byte"] = Label("byte", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 1;
            const ValueSize valSize = BYTE;
            const bool isSigned = false;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("byte");
		}
	});
	AssemblyData["sbyte"] = Label("sbyte", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 1;
            const ValueSize valSize = SBYTE;
            const bool isSigned = true;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("sbyte");
		}
	});
	AssemblyData["word"] = Label("word", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 2;
            const ValueSize valSize = WORD;
            const bool isSigned = false;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("word");
		}
	});
	AssemblyData["sword"] = Label("sword", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 2;
            const ValueSize valSize = SWORD;
            const bool isSigned = true;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("sword");
		}
	});
	AssemblyData["dword"] = Label("dword", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 4;
            const ValueSize valSize = DWORD;
            const bool isSigned = false;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("dword");
		}
	});
	AssemblyData["sdword"] = Label("sdword", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 4;
            const ValueSize valSize = SDWORD;
            const bool isSigned = true;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("sdword");
		}
	});
	AssemblyData["qword"] = Label("qword", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 8;
            const ValueSize valSize = QWORD;
            const bool isSigned = false;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("qword");
		}
	});
	AssemblyData["sqword"] = Label("sqword", [&]() {
		if (isPrepping)
			return;
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[1].type == Label_Value || myLabels[1].type == Label_NULL || myLabels[1].type == Label_Register)) {
            string baseName = myLabels[0].name;
            string extendName = baseName + string(1, '_');
            const int byteCountPerElement = 8;
            const ValueSize valSize = SQWORD;
            const bool isSigned = true;
            int offset1 = AssemblyMemory.size();
            AssemblyData[baseName] = Label(baseName, Value(myLabels[1].value.number, valSize, isSigned), myLabels[1].isInitialized);
            AssemblyData[baseName].offset = offset1;
			myLabels[0] = AssemblyData[baseName];
            //AssemblyMemory.resize(offset1 + byteCountPerElement * (myLabels.size() - 1));
            for(int i = 0; i < byteCountPerElement; i++){
                AssemblyMemory.push_back(baseName);
            }
            for (int i = 2; i < myLabels.size(); i++) {
                string myName = extendName + to_string(i - 1);
                AssemblyData[myName] = Label(myName, Value(myLabels[i].value.number, valSize, isSigned), myLabels[i].isInitialized);
                AssemblyData[myName].offset = offset1 + byteCountPerElement * (i - 1);
                for(int j = 0; j < byteCountPerElement; j++){
                    AssemblyMemory.push_back(extendName + to_string(i - 1));
                }
            }
            AssemblyData["$"].value = AssemblyMemory.size();
		} else if (toLowercase(myInstructs[myInstructIterator + 1]) != "ptr" && myLabels[0].type == Label_NULL) {
			myInstructs.push_back("sqword");
		}
	});
    AssemblyData["type"] = Label("type", [&](){
        if (isPrepping)
            return;
        if (myInstructIterator == 0) {
			myInstructs.push_back("type");
			return;
		}
        
    });
    AssemblyData["jmp"] = Label("jmp", [&]() {
        if (isPrepping)
            return;
        if (myInstructIterator == 0) {
			myInstructs.push_back("jmp");
			return;
		}
		if (myInstructIterator == myInstructs.size() - 1 && (myLabels[0].type == Label_Offset || myLabels[0].type == Label_Function) && isInCodeSegment == myLabels[0].inCodeSegment) {
            myInstructOffset = myLabels[0].offset - 1;
        }
    });
    AssemblyData["loop"] = Label("loop", [&]() {
        if (isPrepping)
            return;
        if (myInstructIterator == 0) {
            myInstructs.push_back("loop");
            return;
        }
        if (myInstructIterator == myInstructs.size() - 1 && (myLabels[0].type == Label_Offset || myLabels[0].type == Label_Function) && isInCodeSegment == myLabels[0].inCodeSegment) {
            AssemblyData["ecx"].value -= 1;
            if (AssemblyData["ecx"].value.number != 0)
                myInstructOffset = myLabels[0].offset - 1;
        }
    });
    AssemblyData["eflags"] = Label("eflags", Value(0, DWORD), true, false, false);
    AssemblyData["cmp"] = Label("cmp", [&]() {
        if (isPrepping)
            return;
        if (myInstructIterator == 0) {
            myInstructs.push_back("loop");
            return;
        }
        if(myInstructIterator == myInstructs.size() - 1){
            int myStartValue = myLabels[0].value.number;
            int myDelta = myLabels[1].value.number;
            int myEndValue = myStartValue - myDelta;

            bool isSigned = myLabels[0].value.isSigned;
            int minValue = isSigned ? -myLabels[0].value.size : 0;
            int maxValue = myLabels[0].value.size - 1;

            int flagBoi = 0;

            //Overflow flag
            if(myDelta > (myStartValue - minValue) || myDelta < (myStartValue - maxValue)){
                flagBoi += 2048;
                if (myDelta > 0) { //Carry flag
                    flagBoi += 1;
                }
            }

            //Zero flag
            if(myEndValue == 0) {
                flagBoi += 64;
            //Sign flag
            } else if (isSigned ? myEndValue < 0 : myEndValue >= (maxValue / 2 + maxValue >= SQWORD)) {
                flagBoi += 128;
            }

            AssemblyData["eflags"].value = flagBoi;
        }
    });
    AssemblyData["je"] = Label("je", [&](){
        if (isPrepping) {
            return;
        }
        if (myInstructIterator == 0) {
            myInstructs.push_back("je");
        } else if (myInstructIterator == myInstructs.size() - 1) {
            if (AssemblyData["eflags"].value.number & 64) {
                myInstructOffset = myLabels[0].offset - 1;
            }
        }
    });
    AssemblyData["jz"] = Label("jz", AssemblyData["je"].handler);
    AssemblyData["jne"] = Label("jne", [&](){
        if (isPrepping) {
            return;
        }
        if (myInstructIterator == 0) {
            myInstructs.push_back("jne");
        } else if (myInstructIterator == myInstructs.size() - 1) {
            if (!(AssemblyData["eflags"].value.number & 64)) {
                myInstructOffset = myLabels[0].offset - 1;
            }
        }
    });
    AssemblyData["jnz"] = Label("jnz", AssemblyData["jne"].handler);
    AssemblyData["jg"] = Label("jg", [&](){
        if (isPrepping) {
            return;
        }
        if (myInstructIterator == 0) {
            myInstructs.push_back("jg");
        } else if (myInstructIterator == myInstructs.size() - 1) {
            int flagBoi = AssemblyData["eflags"].value.number;
            if (((flagBoi & 128) == (flagBoi & 2048)) && !(flagBoi & 64)) {
                myInstructOffset = myLabels[0].offset - 1;
            }
        }
    });
    AssemblyData["ja"] = Label("ja", AssemblyData["jg"].handler);

	// Creating Registers
	for (char i = 'a'; i <= 'd'; i++) {
		string eix = string(1, 'e').append(1, i).append(1, 'x');
		string ix = string(1, i).append(1, 'x');
		string il = string(1, i).append(1, 'l');
		string ih = string(1, i).append(1, 'h');

		AssemblyData[eix] = Label(eix, Value(0, DWORD), true, false, false);
		AssemblyData[ix] = Label(ix, Value(0, WORD), true, false, false);
		AssemblyData[il] = Label(il, Value(0, BYTE), true, false, false);
		AssemblyData[ih] = Label(ih, Value(0, BYTE), true, false, false);
	}

	for (int i = 0; i < 2; i++) {
		isInCodeSegment = false;
		for (myInstructOffset = 0; myInstructOffset < dataInstructions.size(); myInstructOffset++) {
			myInstructs = dataInstructions[myInstructOffset];
			myLabels.clear();
			myLabelIterator = 0;
			for (myInstructIterator = 0; myInstructIterator < myInstructs.size(); myInstructIterator++) {
				Label & currentLabel = GetAssemblyData(AssemblyData, myInstructs[myInstructIterator]);
				if (currentLabel.type == Label_Keyword) {
					currentLabel.handler();
				}
				else {
					if (currentLabel.type == Label_NULL) {
						currentLabel = Label(myInstructs[myInstructIterator]);
					}
					if (myLabelIterator + 1 > myLabels.size()) {
						myLabels.resize(myLabelIterator + 1);
					}
					myLabels[myLabelIterator++] = currentLabel;
				}
			}
		}

		isInCodeSegment = true;
		for (myInstructOffset = 0; myInstructOffset < codeInstructions.size(); myInstructOffset++) {
			myInstructs = codeInstructions[myInstructOffset];
			myLabels.clear();
			myLabelIterator = 0;
			for (myInstructIterator = 0; myInstructIterator < myInstructs.size(); myInstructIterator++) {
				Label & currentLabel = GetAssemblyData(AssemblyData, myInstructs[myInstructIterator]);
				if (currentLabel.type == Label_Keyword) {
					currentLabel.handler();
				}
				else {
					if (currentLabel.type == Label_NULL) {
						currentLabel = Label(myInstructs[myInstructIterator]);
					}
					if (myLabelIterator + 1 > myLabels.size()) {
						myLabels.resize(myLabelIterator + 1);
					}
					myLabels[myLabelIterator++] = currentLabel;
				}
			}
		}

		isPrepping = false;
	}

	return 0;
}

//Constructors
Value::Value(const int & num, const ValueSize & valSize, const bool & sign) {
	_number = sign ? modulo(num + valSize, valSize * 2) - valSize : modulo(num, valSize);
	_size = valSize;
	_isSigned = sign;
}

Value::Value(const Value & otherValue) {
	_number = otherValue.number;
	_size = otherValue.size;
	_isSigned = otherValue.isSigned;
}

//Operator overloads - addition + subtraction
Value Value::operator + (const int & num) {
	return Value(number + num, size, isSigned);
}

Value Value::operator + (const Value & otherValue) {
	return Value(number + otherValue.number, size, isSigned);
}

Value Value::operator - (const int & num) {
	return Value(number - num, size, isSigned);
}

Value Value::operator - (const Value & otherValue) {
	return Value(number - otherValue.number, size, isSigned);
}

//slow ass mofo
// i was eating ffs
//Operator overloads - multiplication + division + modulo
Value Value::operator * (const int & num) {
	return Value(number * num, size, isSigned);
}

Value Value::operator * (const Value & otherValue) {
	return Value(number * otherValue.number, size, isSigned);
}

Value & Value::operator += (const int & num) {
	_number += num;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator += (const Value & otherValue) {
	_number += otherValue.number;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator -= (const int & num) {
	_number -= num;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator -= (const Value & otherValue) {
	_number -= otherValue.number;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator *= (const int & num) {
	_number *= num;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator *= (const Value & otherValue) {
	_number *= otherValue.number;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator /= (const int & num) {
	_number /= num;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

Value & Value::operator /= (const Value & otherValue) {
	_number /= otherValue.number;
	_number = isSigned ? modulo(number + size, size * 2) - size : modulo(number, size);
	return *this;
}

// hey, what about integer division?
// when you divide in assembly doesn't it put the remainder in a flag or something
// bRuH wE'lL gEt ThErE wHeN wE gEt ThErE
//https://stackoverflow.com/a/37010236
Value Value::operator / (const int & num) {
	return Value(number / num, size, isSigned);
}

Value Value::operator / (const Value & otherValue) {
	return Value(number / otherValue.number, size, isSigned);
}

Value Value::operator % (const int & num) {
	return Value(number % num, size, isSigned);
}

Value Value::operator % (const Value & otherValue) {
	return Value(number % otherValue.number, size, isSigned);
}

//Operator overloads - bitwise
Value Value::operator & (const int & num) {
	return Value(number & num, size, isSigned);
}

Value Value::operator & (const Value & otherValue) {
	return Value(number & otherValue.number, size, isSigned);
}

Value Value::operator | (const int & num) {
	return Value(number & num, size, isSigned);
}

Value Value::operator | (const Value & otherValue) {
	return Value(number & otherValue.number, size, isSigned);
}

Value Value::operator << (const int & num) {
	return Value(number << num, size, isSigned);
}

Value Value::operator << (const Value & otherValue) {
	return Value(number << otherValue.number, size, isSigned);
}

Value Value::operator >> (const int & num) {
	return Value(number >> num, size, isSigned);
}

Value Value::operator >> (const Value & otherValue) {
	return Value(number >> otherValue.number, size, isSigned);
}

Value & Value::operator = (const int & num) {
	_number = isSigned ? modulo(num + size, size * 2) - size : modulo(num, size);
	return *this;
}

Value & Value::operator = (const Value & otherValue) {
	_number = otherValue.number;
	_size = otherValue.size;
	_isSigned = otherValue.isSigned;
	return *this;
}

//Member functions
/**
void Value::changeSign(const bool & sign) {
	if(sign != isSigned){
		_isSigned = sign;
		if (sign) { //Unsigned to signed
			_number += size;
			switch(size) {
				case (BYTE):
					_size = SBYTE;
					break;
				case (WORD):
					_size = SWORD;
					break;
				case (DWORD):
					_size = SDWORD;
					break;
				case (QWORD):
					_size = SQWORD;
					break;
				default:
					throw "Invalid size conversion";
			}
		} else { //Signed to unsigned
			switch(size) {
				case (SBYTE):
					_size = BYTE;
					break;
				case (SWORD):
					_size = WORD;
					break;
				case (SDWORD):
					_size = DWORD;
					break;
				case (SQWORD):
					_size = QWORD;
					break;
				default:
					throw "Invalid size conversion";
			}
			_number -= size - 1;
		}
	}
}

int Value::twosComplement() {
	if (isSigned) {
		//Basically just convert the number to unsigned, get twos complement and convert back to signed
		const int sizeUnsigned = size * 2;
		return (sizeUnsigned - (number + size)) % sizeUnsigned - size;
	} else { // wait this only works for unsigned
		return (size - number) % size;
	}
	// invert the bits, add 1
	// 0 0 0 1 1 0 1 0
	// invert it
	// 1 1 1 0 0 1 0 1
	// add 1
	// 1 1 1 0 0 1 1 0
}

**/

//Constructors
Label::Label() { //Label_NULL
	_type = Label_NULL;
	_name = "NULL";
}
Label::Label(const string & newName) { //Label_NULL
	_type = Label_NULL;
	_name = newName;
}
Label::Label(const string & newName, const LabelHandler & newHandler) { //Label_Keyword
	_type = Label_Keyword;
	_name = newName;
	_handler = newHandler;
}
Label::Label(const string & newName, const int & newOffset, const bool & isCode, const bool & isJump) { //Label_Offset and Label_Function
	_type = isJump ? Label_Offset : Label_Function;
	_name = newName;
	offset = newOffset;
	_inDataSegment = !(_inCodeSegment = isCode);
}
Label::Label(const string & newName, const Value & newValue, const bool & init, const bool & noTouchy, const bool & isValue) { //Label_Value and Label_Register
	_type = isValue ? Label_Value : Label_Register;
	_name = newName;
	value = newValue;
	_isInitialized = init || !isValue; //If register, it must be initialized
	_isConstant = noTouchy;
}
Label::Label(const Label & copyFrom) {
	_type = copyFrom.type;
	_name = copyFrom.name;
	switch (type) {
	case Label_NULL:
		break;
	case Label_Keyword:
		_handler = copyFrom.handler;
		break;
	case Label_Offset:
	case Label_Function:
		offset = copyFrom.offset;
		_inDataSegment = !(_inCodeSegment = copyFrom.inCodeSegment);
		break;
	case Label_Value:
	case Label_Register:
		value = copyFrom.value;
		offset = copyFrom.offset;
		_isInitialized = copyFrom.isInitialized;
		_isConstant = copyFrom.isConstant;
		break;
	}
}

//Operator overloads
Label & Label::operator = (const Label & copyFrom) {
	_type = copyFrom.type;
	_name = copyFrom.name;
	switch (type) {
	case Label_NULL:
		break;
	case Label_Keyword:
		_handler = copyFrom.handler;
		break;
	case Label_Offset:
	case Label_Function:
		offset = copyFrom.offset;
		_inDataSegment = !(_inCodeSegment = copyFrom.inCodeSegment);
		break;
	case Label_Value:
	case Label_Register:
		value = copyFrom.value;
		offset = copyFrom.offset;
		_isInitialized = copyFrom.isInitialized;
		_isConstant = copyFrom.isConstant;
		break;
	}
	return *this;
}

//Utility functions
/**
int unsignedToSigned(const int & number, const int & size) {
	const int isSigned = number * 2 / size;
	return number - sign * size;
}
**/

Label & GetAssemblyData(LabelData & AssemblyData, const string & name) {
	char firstCell = name[0];
	if (firstCell >= '0' && firstCell <= '9') {
		int lastCellNum = name.length() - 1;
		char lastCell = name[lastCellNum];
		string num;
		char base = 'd';
		if (lastCell >= '0' && lastCell <= '9') {
			num = name;
		}
		else {
			num = name.substr(0, lastCellNum);
			base = tolower(lastCell);
		}
		//todo?: base 'b' (binary) 'h' (hex)
		int val = static_cast<int>(stoul(num));
		string storedName = (string(1, '$') + num).append(1, base) + "_" + to_string(time(NULL));
		AssemblyData[storedName] = Label(storedName, Value(val, QWORD), true, true);
		return AssemblyData[storedName];
	}
	else {
		string lower = toLowercase(name);
		switch (AssemblyData[lower].type) {
		case Label_Keyword:
		case Label_Register:
			return AssemblyData[lower];
		default:
			return AssemblyData[name];
		}
	}
};

int modulo(const int & x, const int & y) { //idk if we'll even need this but its fine to have it anyways
	if (y >= SQWORD) {
		return x;
	}
	return x - y * static_cast<int>(floor(static_cast<double>(x) / y));
}

string trim(const string & str, const string & whitespace) //https://stackoverflow.com/a/1798170
{
	const int strBegin = str.find_first_not_of(whitespace);
	if (strBegin == string::npos) {
		return ""; // no content
	}

	const int strEnd = str.find_last_not_of(whitespace);
	const int strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

string toLowercase(const string & str) {
	/*
	for(int i = 0; i < str.length(); i++) {
		str[i] = tolower(str[i]);
	}
	*/
	string newStr = str;
	transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
	return newStr;
}

string toUppercase(const string & str) { //Converts a string to uppercase
    /*
	for(int i = 0; i < str.length(); i++) {
		str[i] = toupper(str[i]);
	}
	*/
	string newStr = str;
	transform(newStr.begin(), newStr.end(), newStr.begin(), ::toupper);
	return newStr;
}

vector<string> regex_matches(const string & str, const regex & expression) {
	vector<string> results;

	for (sregex_iterator myMatch = sregex_iterator(str.begin(), str.end(), expression); myMatch != regex_endgame; myMatch++) {
		results.push_back((*myMatch).str());
	}

	return results;
}
