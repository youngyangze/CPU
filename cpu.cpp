#include <bits/stdc++.h>
#include <bits/extc++.h>

using namespace std;
using ll = long long;
using ld = long double;
using ull = unsigned long long;
using vint = vector<int>;
using matrix = vector<vint>;
using vll = vector<ll>;
using matrlx = vector<vll>;
using fourdimensionalMatrix = vector<matrix>;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vpii = vector<pii>;
using vpll = vector<pll>;
using dbl = deque<bool>;
using dbltrix = deque<dbl>;
using sint = stack<int>;
using tii = tuple<int, int, int>;
using ordered_set = __gnu_pbds::tree<int, __gnu_pbds::null_type, less<int>, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;
using cd = complex<double>;

#define fastio ios::sync_with_stdio(false), cin.tie(NULL), cout.tie(NULL)
#define endl '\n'
#define _CRT_SECURE_NO_WARNINGS
#define all(vec) vec.begin(), vec.end()
#define rall(vec) vec.rbegin(), vec.rend()

const int INF = 0x3f3f3f3f;
const ll VINF = 2e18;
const double PI = acos(-1);
constexpr int NUM_REGISTERS = 16;

class file {
public:
    file() {
        registers.fill(0);
    }

    uint16_t& operator[](int index) {
        if (index < 0 || index >= NUM_REGISTERS) throw out_of_range("Register index out of range");
        return registers[index];
    }

    uint16_t getRegister(int index) const {
        if (index < 0 || index >= NUM_REGISTERS) throw out_of_range("Register index out of range");
        return registers[index];
    }

    void setRegister(int index, uint16_t value) {
        if (index < 0 || index >= NUM_REGISTERS) throw out_of_range("Register index out of range");
        registers[index] = value;
    }

    void dump() const {
        cout << "Register Dump:" << endl;
        for (int i = 0; i < NUM_REGISTERS; i++) cout << "R" << i << ": " << registers[i] << endl;
    }

private:
    array<uint16_t, NUM_REGISTERS> registers;
};

enum class OPCODE : uint8_t {
    ADD = 0x01,
    SUB = 0x02,
    MUL = 0x03,
    DIV = 0x04,
    MOV = 0x05,
    LOAD = 0x06,
    STORE = 0x07,
    // do it lol
    NOP = 0x00
};

struct operation {
    OPCODE opcode;
    uint8_t dest, src1, src2;

    operation() : opcode(OPCODE::NOP), dest(0), src1(0), src2(0) {}
};

struct VLIWInstruction {
    operation op1;
    operation op2;

    VLIWInstruction() = default;
};

VLIWInstruction decodeInstruction(uint32_t instruction) {
    VLIWInstruction vliw;

    uint8_t op1code = (instruction >> 24) & 0xFF, op1dest = (instruction >> 20) & 0x0F, op1src1 = (instruction >> 16) & 0x0F, op1src2 = (instruction >> 12) & 0x0F;
    vliw.op1.opcode = static_cast<OPCODE>(op1code);
    vliw.op1.dest = op1dest;
    vliw.op1.src1 = op1src1;
    vliw.op1.src2 = op1src2;

    uint8_t op2code = (instruction >> 8) & 0x0F, op2dest = (instruction >> 4) & 0x0F, op2src1 = (instruction) & 0x0F;
    vliw.op2.opcode = static_cast<OPCODE>(op2code);
    vliw.op2.dest = op2dest;
    vliw.op2.src1 = op2src1;
    vliw.op2.src2 = 0; // unused

    return vliw;
}

class CPU {
public:
    CPU() : PC(0), halted(false) {}

    void loadProgram(const vector<uint32_t>& program) {
        this->program = program;
    }

    void run(bool debug = false) {
        while (!halted && PC < program.size()) {
            uint32_t instruction = program[PC];
            VLIWInstruction vliw = decodeInstruction(instruction);

            if (debug) cout << "Executing instruction at PC=" << PC << ": 0x" << hex << instruction << dec << endl << "  Operation 1: " << opcodeToString(vliw.op1.opcode) << " R" << (int)vliw.op1.dest << ", R" << (int)vliw.op1.src1 << ", R" << (int)vliw.op1.src2 << endl << "  Operation 2: " << opcodeToString(vliw.op2.opcode) << " R" << (int)vliw.op2.dest << ", R" << (int)vliw.op2.src1 << endl;

            executeOperation(vliw.op1, debug);
            executeOperation(vliw.op2, debug);
            PC++;
        }

        if (debug) registerFile.dump();
    }

    void halt() {
        halted = true;
    }

    void setRegister(int index, uint16_t value) {
        registerFile.setRegister(index, value);
    }

    uint16_t getRegister(int index) const {
        return registerFile.getRegister(index);
    }

    void dumpRegisters() const {
        registerFile.dump();
    }

private:
    file registerFile;
    vector<uint32_t> program;
    size_t PC;
    bool halted;

    void executeOperation(const operation &op, bool debug) {
        switch (op.opcode) {
            case OPCODE::ADD:
                registerFile.setRegister(op.dest, registerFile.getRegister(op.src1) + registerFile.getRegister(op.src2));
                if (debug) cout << "    ADD R" << (int)op.dest << " = R" << (int)op.src1 << " + R" << (int)op.src2 << endl;
                break;
            case OPCODE::SUB:
                registerFile.setRegister(op.dest, registerFile.getRegister(op.src1) - registerFile.getRegister(op.src2));
                if (debug) cout << "    SUB R" << (int)op.dest << " = R" << (int)op.src1 << " - R" << (int)op.src2 << endl;
                break;
            case OPCODE::MUL:
                registerFile.setRegister(op.dest, registerFile.getRegister(op.src1) * registerFile.getRegister(op.src2));
                if (debug) cout << "    MUL R" << (int)op.dest << " = R" << (int)op.src1 << " * R" << (int)op.src2 << endl;
                break;
            case OPCODE::DIV:
                if (registerFile.getRegister(op.src2) == 0) {
                    cerr << "Division by zero at PC=" << PC << endl;
                    halted = true;
                } else {
                    registerFile.setRegister(op.dest, registerFile.getRegister(op.src1) / registerFile.getRegister(op.src2));
                    if (debug) cout << "    DIV R" << (int)op.dest << " = R" << (int)op.src1 << " / R" << (int)op.src2 << endl;
                }
                break;
            case OPCODE::MOV:
                registerFile.setRegister(op.dest, registerFile.getRegister(op.src1));
                if (debug) cout << "    MOV R" << (int)op.dest << " = R" << (int)op.src1 << endl;
                break;
            case OPCODE::LOAD:
                registerFile.setRegister(op.dest, memory[registerFile.getRegister(op.src1)]);
                if (debug) cout << "    LOAD R" << (int)op.dest << " = MEM[R" << (int)op.src1 << "]\n";
                break;
            case OPCODE::STORE:
                memory[registerFile.getRegister(op.src1)] = registerFile.getRegister(op.dest);
                if (debug) cout << "    STORE MEM[R" << (int)op.src1 << "] = R" << (int)op.dest << endl;
                break;
            case OPCODE::NOP:
                if (debug) cout << "    NOP" << endl;
                break;
            default:
                cerr << "Unknown opcode at PC=" << PC << endl;
                halted = true;
        }
    }

    string opcodeToString(OPCODE opcode) const {
        static unordered_map<OPCODE, string> opcodeMap = {
            {OPCODE::ADD, "ADD"},
            {OPCODE::SUB, "SUB"},
            {OPCODE::MUL, "MUL"},
            {OPCODE::DIV, "DIV"},
            {OPCODE::MOV, "MOV"},
            {OPCODE::LOAD, "LOAD"},
            {OPCODE::STORE, "STORE"},
            {OPCODE::NOP, "NOP"}
        };

        auto it = opcodeMap.find(opcode);
        if (it != opcodeMap.end()) return it->second;
        return "UNKNOWN";
    }

    array<uint16_t, 1 << NUM_REGISTERS> memory = {0};
};

int main() { // i don't actually do example code but it's hardone so i had to do this
    fastio;
    CPU cpu;

    // Instruction 1: ADD R0 = R1 + R2, NOP
    // Instruction 2: SUB R3 = R0 - R1, NOP

    // Instruction format: [op1code(8)][op1dest(4)][op1src1(4)][op1src2(4)]
    //                     [op2code(4)][op2dest(4)][op2src1(4)][op2src2(0)]
    // Example: ADD R0 = R1 + R2, NOP
    uint32_t add_nop = (static_cast<uint32_t>(OPCODE::ADD) << 24) |
                       ((uint32_t)0 << 20) | // dest R0
                       ((uint32_t)1 << 16) | // src1 R1
                       ((uint32_t)2 << 12) | // src2 R2
                       ((static_cast<uint32_t>(OPCODE::NOP) & 0x0F) << 8) |
                       ((uint32_t)0 << 4) |  // dest R0 for op2 (unused)
                       ((uint32_t)0);        // src1 R0 for op2 (unused)

    // Instruction 2: SUB R3 = R0 - R1, NOP
    uint32_t sub_nop = (static_cast<uint32_t>(OPCODE::SUB) << 24) |
                       ((uint32_t)3 << 20) | // dest R3
                       ((uint32_t)0 << 16) | // src1 R0
                       ((uint32_t)1 << 12) | // src2 R1
                       ((static_cast<uint32_t>(OPCODE::NOP) & 0x0F) << 8) |
                       ((uint32_t)0 << 4) |  // dest R0 for op2 (unused)
                       ((uint32_t)0);        // src1 R0 for op2 (unused)

    vector<uint32_t> program = { add_nop, sub_nop };
    cpu.loadProgram(program);

    cpu.setRegister(1, 5);
    cpu.setRegister(2, 10);

    cout << "Initial Register State:" << endl;
    cpu.dumpRegisters();

    cout << endl << "Running Program with Debug Mode:" << endl;
    cpu.run(true);

    return 0;
}
