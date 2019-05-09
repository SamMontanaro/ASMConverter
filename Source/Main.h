#ifndef MAIN_H
#define MAIN_H

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

using namespace std;

//Utility consts
const sregex_iterator regex_endgame; //Thanos dies in Endgame of Thrones
const string Label_Keywords[] = { "[", "]", "+", "-", ",", "mov", "add", "sub", "inc", "dec", "mul", "imul", "div", "idiv", "$", "byte", "word", "dword", "qword", "sbyte", "sword", "sdword", "sqword", "offset", "proc", "end", "dup", "xchg", "neg", "ptr", "type", "lengthof", "sizeof", "jmp", "loop", "movzx", "movsx", "ja", "jb", "jae", "jbe", "jna", "jnb", "jnae", "jnbe", "jg", "jl" "jge", "jle", "jne", "jnl", "jng", "jlne", "jgne", "je", "jz", "jnz", "jc", "jnc", "jo", "jno", "js", "jns", "jp", "jnp", "jcxz", "jecxz", "cmp", "test", "or", "and", "xor", "not", "stc", "clc", "call", "pushfd", "popfd", "pushad", "popad", "push", "pop", "lahf", "sahf", "shl", "sal", "shr", "sar", "rol", "ror", "shld", "shrd", "cbw", "cwd", "cdq", "adc", "sbb", "END_OF_KEYWORDS" }; //you have been promoted from pussy to bitch ass but you should still add more :))))
// WOW THAT'S LIKE ALL OF THEM, YOU A PUSSY

//Typedef YEET
class Label;
typedef unordered_map<string, Label> LabelData;
typedef function<void(/*vector<string> &, int &, LabelData &*/)> LabelHandler;

//Utility function prototypes
Label & GetAssemblyData(LabelData &, const string &);
int modulo(const int &, const int &); //True modulo unlike % operator
string trim(const string &, const string & = " \t"); //Filters out whitespace (or any provided filter characters) from the start and end of a string
string toLowercase(const string &); //Converts a string to lowercase
string toUppercase(const string &); //Converts a string to uppercase
vector<string> regex_matches(const string &, const regex &); //Puts every match of a regex in a string into the vector
/**
int unsignedToSigned(int, int); //Convert an unsigned int to a signed int
int signedToUnsigned(int, int); //Convert a signed int to an unsigned int
**/

enum ValueSize { //Non-negative/non-positive ranges for different types (minus 1 for positive limit)
	//Unsigned ranges
	BYTE = 256, //2^8,
	WORD = 65536, //2^16,
	DWORD = 4294967296, //2^32,
    QWORD = 18446744073709551615, //2^64 - 1,

	//Signed ranges
	SBYTE = 128, //2^7,
	SWORD = 32768, //2^15,
	SDWORD = 2147483648, //2^31,
	SQWORD = 9223372036854775807 //2^63 - 1
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
    Value(const Value &);

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

	Value & operator = (const int &);
	Value & operator = (const Value &);

	Value & operator += (const int &);
	Value & operator += (const Value &);
	Value & operator -= (const int &);
	Value & operator -= (const Value &);
	Value & operator *= (const int &);
	Value & operator *= (const Value &);
	Value & operator /= (const int &);
	Value & operator /= (const Value &);

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

enum LabelType {
	Label_Keyword,//Includes instructions like mov, etc. will have the keyword type
	Label_Offset, //Refers to line numbers really, like a jump label created by the user would be of this type
	Label_Value, //A variable usually
	Label_Register, //It's a label for a register... like a label is basically a name for something, so each register will have a label associated with it. EAX is a label for example. The point of having a struct for this is so that we can have a dictionary of labels and their values and our code can look up variable names or label names or etc.
	Label_Function, //A special offset
	Label_NULL //This label does not exist YIKES
};

//http://www.cplusplus.com/reference/map/map/
// hey joe, there's an unordered_map, do we really need this to be organized, apparently unordered is faster wdym ordered
//oh wait nvm, yeah we can use unordered_map HAHAHAHA TY
class Label {
	//_name indicates backend variable like in Value
	LabelType _type = Label_NULL;
	string _name;

	//For Label_Keyword
	LabelHandler _handler;

	//For Label_Offset and Label_Function
	bool _inDataSegment = false;
	bool _inCodeSegment = false;

	//For Label_Value and Label_Register
	bool _isInitialized = false;
	bool _isConstant = false;
	//those read only members thooooo yeeeee
public:
	//Read only members
	const LabelType & type = _type;
	const string & name = _name;
	const LabelHandler & handler = _handler;
	int offset = 0;
	const bool & inDataSegment = _inDataSegment;
	const bool & inCodeSegment = _inCodeSegment;
	const bool & isInitialized = _isInitialized;
	const bool & isConstant = _isConstant;
	Value value;

	//Constructors
	Label(); //Label_NULL
	Label(const string &); //Label_NULL
	Label(const string &, const LabelHandler &); //Label_Keyword
	Label(const string &, const int &, const bool & = true, const bool & = true); //Label_Offset and Label_Function
	Label(const string &, const Value &, const bool & = false, const bool & = false, const bool & = true); //Label_Value and Label_Register
	Label(const Label &); //Copy constructor

	//Operator overloads
	Label & operator = (const Label &);

	//This shit is so fucking stupid
	~Label() {};
};

struct Keyword {
	string name;
	LabelHandler handler;

	// this should be here, at least the declaration

};

#endif
