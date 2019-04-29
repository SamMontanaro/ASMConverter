#include <iostream> //For strings
#include <fstream> //For files
#include <string> //For strings
#include <stack> //For stacks
#include <vector> //For dynamic arrays, needed mostly for line numbers
#include <cmath> //For floor, might remove
#include <regex> //For syntax parsing
#include <unordered_map> //For storing labels and values/registers/etc.

using namespace std;

/*
9:58 PM
Technidecimal
HECCIN DUNNIT
we gucci in the hucci
let's call it a night



we're at 11:44 PM now
*/

//Utility consts
const sregex_iterator regex_endgame; //Thanos confirmed dead

// WOW THAT'S LIKE ALL OF THEM, YOU A PUSSY

//Utility function prototypes
int modulo(const int &, const int &); //True modulo unlike % operator
string trim(const string &, const string & = " \t"); //Filters out whitespace (or any provided filter characters) from the start and end of a string
string toLowercase(const string &); //Converts a string to lowercase
vector<string> regex_matches(const string &, const regex &); //Puts every match of a regex in a string into the vector
/**
int unsignedToSigned(int, int); //Convert an unsigned int to a signed int
int signedToUnsigned(int, int); //Convert a signed int to an unsigned int
**/

enum ValueSize { //Non-negative/non-positive ranges for different types (minus 1 for positive limit)
    //Unsigned ranges
    BYTE = 2^8,
    WORD = 2^16, 
    DWORD = 2^32,
    QWORD = 2^64,

    //Signed ranges
    SBYTE = 2^7,
    SWORD = 2^15,
    SDWORD = 2^31,
    SQWORD = 2^63
};

class Value { //A value...
    //Backend members
        int _number;
        ValueSize _size;
        bool _isSigned;

    public:
        //READ-ONLY MEMBERS YEEEEEEEEEEEEEEET
        const int & number = _number;
        const ValueSize & size = _size;
        const bool & isSigned = _isSigned;

        //Constructor prototypes
        Value(const int & = 0, const ValueSize & = BYTE, const bool & = false); 

        //Operator overload prototypes
        Value operator + (const int &); 
        Value operator + (const Value &); 
        Value operator - (const int &);
        Value operator - (const Value &);

        Value operator * (const int &); 
        Value operator * (const Value &); 
        Value operator / (const int &);
        Value operator / (const Value &);
        Value operator % (const int &);
        Value operator % (const Value &);

        Value operator & (const int &);
        Value operator & (const Value &);
        Value operator | (const int &);
        Value operator | (const Value &);
        Value operator << (const int &);
        Value operator << (const Value &);
        Value operator >> (const int &);
        Value operator >> (const Value &);

        
        Value operator = (const int &);
        Value operator = (const Value &);

        //Member function prototypes
        /**
        void changeSign(const bool & = false);
        int twosComplement();
        int unsignedToSigned(const int &, const int &) {
        **/
};

/*
    32 Bit General Purpose Registers:
    EAX, EBX, ECX, EDX
    
    (ECX is the loop counter)

    EBP - Extended frame pointer (stack), references parameters and local variables on the stack (push/pop from stack)
    
    ESP - Stack pointer, points to the last address used in the stack
    
    Index Registers - Used by high-speed memory transfer instructions (change addresses of operands):
        ^^
    ESI ^^
    EDI ^^

    EFLAGS - Equal to ODITSZRARPRC
    O = Overflow flag (if the last mathematical operation resulted in an overflow)
    D = Direction flag (nobody cares set it to 0)
    I = Interrupt enable flag (ew go away set it to 0)
    T = Trap flag (it's gay so set it to 0)
    S = Sign flag (the value of the MSB in the result of the last mathematical operation)
    Z = Zero flag (whether the last mathematical operation resulted in 0)
    R = Reserved flag (reserve a 0 for this bitch ass)
    A = Adjust flag (same as carry flag but as if the result was only the 4 LSB)
    P = Parity flag (whether the last mathematical operation resulted in an even number of bits being set to 1)
    C = Carry Flag (whether a carry/borrow was generated out of the MSB of the result)

    EIP - Contains address of next instruction to be executed

    16 Bit Segment Registers:
    CS - Code Segment       ES - "Additional Segments"
    SS - Stack Segment      FS - "Additional Segments"
    DS - Data Segment       GS - "Additional Segments"
*/

class Register {
        Value value;


};

enum LabelType {
    Label_Keyword,//Includes instructions like mov, etc. will have the keyword type
    Label_Offset, //Refers to line numbers really, like a jump label created by the user would be of this type
    Label_Value, //A variable usually
    Label_Register, // It's a label for a register... like a label is basically a name for something, so each register will have a label associated with it. EAX is a label for example. The point of having a struct for this is so that we can have a dictionary of labels and their values and our code can look up variable names or label names or etc.
    Label_Function //A special offset
};

//http://www.cplusplus.com/reference/map/map/
// hey joe, there's an unordered_map, do we really need this to be organized, apparently unordered is faster wdym ordered 
//oh wait nvm, yeah we can use unordered_map HAHAHAHA TY
class Label {
        //_name indicates backend variable like in Value
        LabelType _type = Label_Value;
        string _name;

        //For Label_Register
        Register _reg; 

        //For Label_Offset and Label_Function
        int _offset = 0;
        bool _inDataSegment = false;
        bool _inCodeSegment = false;
        
        //For Label_Value
        bool _isInitialized = false;
        Value _value;
//those read only members thooooo yeeeee
    public:
        //Read only members
        const LabelType & type = _type;
        const string & name = _name;
        const Register & reg = _reg;
        const int & offset = _offset;
        const bool & inDataSegment = _inDataSegment;
        const bool & inCodeSegment = _inCodeSegment;
        const bool & isInitialized = _isInitialized;
        const Value & value = _value;

        //Constructors
        Label(const string &); //Label_Keyword
        Label(const string &, int offset &, const bool & = true, const bool & = true); //Label_Offset and Label_Function
        Label(const string &, const Value &, const bool &); //Label_Value
        Label(const string &, const Register &); //Label_Register
};

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
        regex expression("\\S+");
        sregex_iterator endCondition;
        string line;
        
        getline(assemblyInput, line); // Pumping and priming
        while(!(assemblyInput.eof() || assemblyInput.fail())){
            commentPos = (line + ";").find_first_of(";");
            line = trim(line.substr(0, commentPos));

            if (line != "") {
                if (line == ".data") {
                    inDataSegment = !(inCodeSegment = false);
                } else if (line == ".code") {
                    inCodeSegment = !(inDataSegment = false);
                } else if (line[0] == '.') {
                    inDataSegment = (inCodeSegment = false);
                } else if (inDataSegment || inCodeSegment) {
                    vector<string> words = regex_matches(line, expression);
                    if(words.size() > 0) {
                        (inDataSegment ? dataInstructions : codeInstructions).push_back(words);
                    }
                }
            }

            getline(assemblyInput, line);
        }
    }

    //Okay, now that we've populated the data and code instructions, it's time to start executing them
    //jk we actually have to scan through them once first to save all labels and functions
    //jk again we need to initialize our own shit first

    unordered_map<string, Label> AssemblyData;
    { //Encapsulate temp variables again
        const string Label_Keywords[] = {"mov", "add", "sub", "inc", "dec", "mul", "imul", "div", "idiv", "$", "byte", "word", "dword", "qword", "sbyte", "sword", "sdword", "sqword", "offset", "proc", "end", "dup", "xchg", "neg", "ptr", "type", "lengthof", "sizeof", "jmp", "loop", "movzx", "movsx", "ja", "jb", "jae", "jbe", "jna", "jnb", "jnae", "jnbe", "jg", "jl" "jge", "jle", "jne", "jnl", "jng", "jlne", "jgne", "je", "jz", "jnz", "jc", "jnc", "jo", "jno", "js", "jns", "jp", "jnp", "jcxz", "jecxz", "cmp", "test", "or", "and", "xor", "not", "stc", "clc", "call", "pushfd", "popfd", "pushad", "popad", "push", "pop", "lahf", "sahf", "shl", "sal", "shr", "sar", "rol", "ror", "shld", "shrd", "cbw", "cwd", "cdq", "adc", "sbb", "END_OF_KEYWORDS"}; //you have been promoted from pussy to bitch ass but you should still add more :))))
        
        int i = 0;
        string & v;
        while ((v = Label_Keywords[i++]) != "END_OF_KEYWORDS") {
            AssemblyData[v] = Label(v);
        }
    }

    cout << toLowercase("P0rnhub.com") << "\nGoodbye to your pussy ass!";

    return 0;
}

//Constructors
Value::Value(const int & num, const ValueSize & valSize, const bool & sign) {
    _number = sign ? modulo(num + valSize, valSize * 2) - valSize : modulo(num, valSize);
    _size = valSize;
    _isSigned = sign;
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
// i was eating you fucker
//Operator overloads - multiplication + division + modulo
Value Value::operator * (const int & num) {
    return Value(number * num, size, isSigned);
} 

Value Value::operator * (const Value & otherValue) {
    return Value(number * otherValue.number, size, isSigned);
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

Value Value::operator = (const int & num) {
    _number = isSigned ? modulo(num + size, size * 2) - size : modulo(num, size);
    return *this;
}

Value Value::operator = (const Value & otherValue) {
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
Label::Label(const string & newName) { //Label_Keyword
    _type = Label_Keyword;
    _name = newName;
} 
Label::Label(const string & newName, int newOffset &, const bool & isCode, const bool & isJump){ //Label_Offset and Label_Function
    _type = isJump ? Label_Offset : Label_Function;
    _name = newName;
    _offset = newOffset;
    _inDataSegment = !(_inCodeSegment = isCode);
}
Label::Label(const string & newName, const Value & newValue, const bool & init){ //Label_Value
    _type = Label_Value;
    _name = newName;
    _value = newValue;
    _isInitialized = init;
}
Label::Label(const string & newName, const Register & newRegister){ //Label_Register
    _type = Label_Register;
    _name = newName;
    _register = newRegister;
}

//Utility functions
/**
int unsignedToSigned(const int & number, const int & size) {
    const int isSigned = number * 2 / size;
    return number - sign * size;
}
**/

int modulo(const int & x, const int & y) { //idk if we'll even need this but its fine to have it anyways
    return x - y * floor(y / x);
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

string toLowercase(const string & str){
    /*
    for(int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
    */
    string newStr = str;
    transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
    return newStr;
}

vector<string> regex_matches(const string & str, const regex & expression) {
    vector<string> results;

    sregex_iterator myMatch(str.begin(), str.end(), expression);
    if(myMatch != regex_endgame) {
        do {
            results.push_back((*myMatch).str());
            ++myMatch;
        } while (myMatch != regex_endgame);
    }

    return results;
}
