#include <unordered_set>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <math.h>
#include <cstring>

using namespace std;

int linenum = 1;
int lineoffset = 0;
int module_number = 0;
//Global Symbol Table

int errors = 0;
int warnings = 0;

int prevnum = 0;
int prev_charoffset = 0;
int char_offset = 0;

map<string,int> symbol_table;
vector<pair<string,int>> symbol_t;
vector<int> module_size;

unordered_map<string, vector<string>> symbol_error;
vector<string> warnings_pre_pass1;
unordered_map<string,int> used_e_instruction;

unordered_map<string, int> symbol_module_number;
unordered_set<string> symbol_used;

unordered_set<char> marie_symbols = {'M', 'A', 'R', 'I', 'E'};
unordered_set<string> symbols_defined;
FILE *input_file = NULL;



void __syntax_error(int errcode) {
    vector<string> error_str = {
    "TOO_MANY_DEF_IN_MODULE", // > 16
    "TOO_MANY_USE_IN_MODULE", // > 16
    "TOO_MANY_INSTR",
    "MARIE_EXPECTED",
    "NUM_EXPECTED",
    "SYM_EXPECTED",
     "SYM_TOO_LONG"
    };

    printf("Syntax Error line %d offset %d: %s\n", linenum, lineoffset, error_str[errcode].c_str());
    exit(1); 
}


 char* get_token() {
     char* token = (char*)malloc(512);
     char c;

     if (token == NULL) {
         return NULL;
     }

     int next_char_index = 0;

     while((c = fgetc(input_file)) != EOF) {
         if (c == ' ') {
            prev_charoffset = char_offset;
             char_offset += 1;
             lineoffset += 1;
             continue;
         }

         if(c == '\t'){
            prev_charoffset = char_offset;
            char_offset += 1;
            lineoffset += 1;
            continue;
         }

         if (c == '\n') {
             prevnum = linenum;
             prev_charoffset = char_offset;
             linenum += 1;
             lineoffset = 0;
             char_offset = 0;
             continue;
         }

         if (isalnum(c)) {
             ungetc(c, input_file);
             char curr_char;
             while ((curr_char = fgetc(input_file)) != EOF && next_char_index < 511) {
                 if (isalnum(curr_char)) {
                     token[next_char_index] = curr_char;
                     next_char_index += 1;
                     prev_charoffset = char_offset;
                     char_offset += 1;
                 } else {
                     ungetc(curr_char, input_file);
                     break;
                 }
             }
              token[next_char_index] = '\0';
              prevnum = linenum;
              lineoffset += 1;
              return token;
         }
     }

     free(token);
     linenum = prevnum;
     lineoffset = prev_charoffset+1;


     return NULL;
 }

bool token_nullcheck(char* token){
    return (token == nullptr);
}

int read_int(char *token) {
    if (token_nullcheck(token)){
        __syntax_error(4); 
        exit(1);
    }

    auto i = atoi(token);
    if (i >= pow(2,30)){
        __syntax_error(4); 
        exit(1);
    }

    string tok = string(token);
    for(unsigned int i = 0; i < strlen(token); i++){
        if (!isdigit(tok[i])){
            __syntax_error(4); 
            exit(1);
        }
    }
    return atoi(token);
}

string read_symbol(char * token) {
    if (token_nullcheck(token)) {
        __syntax_error(5);
        exit(1);
    }

    if (isdigit(token[0])){
        __syntax_error(5);
        exit(1);
    }
    if (strlen(token) > 16){
        __syntax_error(6);
        exit(1);
    }

    string tok = string(token);

    return tok;
}

string read_marie(char* token) {
    if (token_nullcheck(token)){
        __syntax_error(3); 
        exit(1);
    } 

    if (strlen(token) > 1){
        __syntax_error(3); 
        exit(1);
    }

    string tok = string(token);

    if (marie_symbols.find(tok[0]) == marie_symbols.end()){
        __syntax_error(3);
        exit(1);
    }
    return tok;
}

 map<string,int> Pass1() {

     int location_counter = 0;
     int module_number = 0;
     int total_file_instructions = 0;
     unordered_map<string, int> relative_word_address;
     vector<string> symbol_define_in_module;
     vector<int> module_sizes;

     char *tok;

     while((tok = get_token()) != NULL) {
        
         int def_count = 0;
         int def_count_max = read_int(tok);

         if (def_count_max > 16){
             __syntax_error(0);
             exit(1);
         }

         while (def_count < def_count_max) {
             char *c1 = get_token();
             char *c2 = get_token();

             string symbol = read_symbol(c1);
             int address =  read_int(c2);

             if (symbols_defined.find(symbol) != symbols_defined.end()){
                string error = "Error: This variable is multiple times defined; first value used";
                string warning = "Warning: Module " + to_string(module_number) + ": "+ symbol + " redefinition ignored";
                warnings_pre_pass1.push_back(warning);
                symbol_error[symbol].push_back(error);
             } else {
                symbol_module_number[symbol] = module_number;
                symbols_defined.insert(symbol);
                symbol_table[symbol] = address + location_counter;
                symbol_t.push_back({symbol, address + location_counter});
                relative_word_address[symbol] = address;
             }

             def_count = def_count + 1;

             free(c1);
             free(c2);
         }

         int use_count = 0;
         char *use_count_tok = get_token();
         int use_count_max = read_int(use_count_tok);

         if (use_count_max > 16) {
             __syntax_error(1);
             free(use_count_tok);
             return symbol_table;
         }

         while(use_count < use_count_max){
             char *c = get_token();
             if (c == NULL) {
                __syntax_error(4);
                 exit(1);
             }
             string symbol = read_symbol(c);
             symbol_used.insert(symbol);
             use_count = use_count + 1;
             free(c);
         }

         free(use_count_tok);
         int code_count = 0;
         char *code_count_tok = get_token();
         int code_count_max = read_int(code_count_tok);

         if (code_count_max+total_file_instructions > 512) {
             __syntax_error(2);
             free(code_count_tok);
             return symbol_table;
         }
         total_file_instructions += code_count_max;
         free(code_count_tok);
         module_size.push_back(code_count_max);

         while(code_count < code_count_max){

             char *c = get_token();
             if (c == NULL) {
                 __syntax_error(3);
                 exit(1);
             }
            
             char *c1 = get_token();
             if (c1 == NULL) {
                __syntax_error(4);
                exit(1);
             }

             read_marie(c);
             read_int(c1);
             code_count = code_count + 1;
             free(c);
             free(c1);
         }

         free(tok);

         for (auto pair : symbol_module_number){
            if (pair.second == module_number){
                int add = relative_word_address[pair.first];
                if (add > code_count_max){
                    printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative\n", module_number,pair.first.c_str(),relative_word_address[pair.first],code_count_max-1);
                    warnings += 1;
                    symbol_table[pair.first] = location_counter;
                }
            }
         }
         module_number += 1;
         location_counter = location_counter + code_count_max;
     }
     return symbol_table;
 }

vector<pair<int,int>> Pass2() {

    int location_counter = 0;
    int instruction_number = 0;
    vector<pair<int,int>> binary_code;
    vector<string> external_module_symbols;
    unordered_set<string> e_module_symbols;

    int module_no = 0;
    char *tok;
    while ((tok = get_token()) != NULL){

        int def_count_max = atoi(tok);
        free(tok);

        for(int i = 0; i < def_count_max; i++){
            char *symbol_tok = get_token();
            char *addr_tok = get_token();
            
            string symbol = read_symbol(symbol_tok);
            read_int(addr_tok);

            free(symbol_tok);
            free(addr_tok);
        }


        int use_count_max = atoi(get_token());

        for(int i = 0; i < use_count_max; i++){
            char *tok = get_token();
            string t = read_symbol(tok);
            external_module_symbols.push_back(t);
            e_module_symbols.insert(t);
            free(tok);
        }

        int code_count_max = atoi(get_token());

        for(int i = 0; i < code_count_max; i++){
            char *tok_instruction = get_token();
            char *token_address = get_token();

            string instruction = read_marie(tok_instruction);
            int address = read_int(token_address);


            int opcode = address/1000;
            if (opcode >= 10){
                address = 9999;
                string error = "Error: Illegal opcode; treated as 9999";
                symbol_error[to_string(instruction_number)].push_back(error);
            }

            int operand = address - (opcode * 1000);

            if (instruction == "A") {
                if (operand >= 512) {
                    address = (opcode * 1000);
                    string error = "Error: Absolute address exceeds machine size; zero used";
                    symbol_error[to_string(instruction_number)].push_back(error);
                }

                binary_code.push_back({instruction_number, address});
                instruction_number += 1;

            } else if (instruction == "E") {
                if (operand >= int(external_module_symbols.size())){
                    string error = "Error: External operand exceeds length of uselist; treated as relative=0";
                    symbol_error[to_string(instruction_number)].push_back(error);

                    address = (opcode * 1000);
                } else {
                    string s = external_module_symbols[operand];
                    if(symbol_table.find(s) == symbol_table.end()) {
                        string error = "Error: "+s+" is not defined; zero used";
                        symbol_error[to_string(instruction_number)].push_back(error);
                        address = (opcode * 1000);
                    } else {
                        address = (opcode * 1000) + symbol_table[s];
                        used_e_instruction[s] = instruction_number;
                    }
                    e_module_symbols.erase(s);
                }
                
                binary_code.push_back({instruction_number, address});
                instruction_number += 1;

            } else if (instruction == "R") {
                if (operand >= code_count_max){
                    operand = 0;
                    int global_address = (opcode * 1000) + location_counter;
                    string error = "Error: Relative address exceeds module size; relative zero used";
                    symbol_error[to_string(instruction_number)].push_back(error);
                    binary_code.push_back({instruction_number, global_address});
                    instruction_number += 1;
                } else {
                    int global_address = location_counter + operand;
                    global_address = (opcode * 1000) + global_address;
                    binary_code.push_back({instruction_number, global_address});
                    instruction_number += 1;
                }
            } else if (instruction == "I"){
                if (operand >= 900){
                    string error = "Error: Illegal immediate operand; treated as 999";
                    int global_address = (opcode * 1000) + 999;
                    symbol_error[to_string(instruction_number)].push_back(error);
                    binary_code.push_back({instruction_number, global_address});
                    instruction_number += 1;
                } else {
                    int global_address = (opcode * 1000) + operand;
                    binary_code.push_back({instruction_number, global_address});
                    instruction_number += 1;
                }
            } else if (instruction == "M"){
                if (operand > int(module_size.size())-1){
                    string error = "Error: Illegal module operand ; treated as module=0";
                    address = (opcode * 1000);
                    symbol_error[to_string(instruction_number)].push_back(error);
                    binary_code.push_back({instruction_number, address});
                    instruction_number += 1;


                } else {
                    int target_module_base = 0;
                    for (int i = 0; i < operand; i++) {target_module_base += module_size[i];}
                    int global_address = (opcode * 1000) + target_module_base;
                    binary_code.push_back({instruction_number, global_address});
                    instruction_number += 1;
                }

            }

        }

        for(int i = 0; i < int(external_module_symbols.size()); i++) {
            string symbol = external_module_symbols[i];
            if (e_module_symbols.find(symbol) != e_module_symbols.end()) {
                string error = "Warning: Module " + to_string(module_no) + ": uselist[" + to_string(i) + "]=" + symbol + " was not used";
                if (instruction_number - 1 >= 0){
                    symbol_error[to_string(instruction_number-1)].push_back(error);
                } else {
                    symbol_error[0].push_back(error);
                }
            }
        }
        module_no += 1;
        location_counter = location_counter + code_count_max;
        external_module_symbols.clear();
        e_module_symbols.clear();
    }
    return binary_code;
}

int main(int argc, char* argv[]) {
    input_file = fopen(argv[1], "r");

    if (argc < 2){
        printf("More inputs expected\n");
        exit(1);
    }

    map<string,int> s = Pass1();

    linenum = 1;
    lineoffset = 0;
    input_file = fopen(argv[1], "r");

    vector<pair<int,int>> s1 = Pass2();

    for(auto s : warnings_pre_pass1){
        printf("%s\n", s.c_str());
        warnings += 1;
    }

    printf("SymTable:\n");
    for (auto pair : symbol_t){
        if (symbol_error.find(pair.first.c_str()) != symbol_error.end()){
            printf("%s=%d ", pair.first.c_str(), pair.second);
            for (auto c : symbol_error[pair.first.c_str()]){
                string construct = c + "\n";
                printf("%s",construct.c_str());
            }
            errors += 1;
        } else {
            printf("%s=%d\n", pair.first.c_str(), symbol_table[pair.first]);
        }
    }


    printf("\n");

    printf("BinaryCode:\n");


    bool prev_warning = false;

    for (auto pair : s1){
        int first = pair.first;
        if (symbol_error.find(to_string(first)) != symbol_error.end()){
            printf("%03d: %04d", pair.first, pair.second);
            for (auto c : symbol_error[to_string(first)]){
                string construct = c + "\n";
                
                if (c.find("Warning:") != string::npos) {
                    printf("\n");
                    warnings += 1;
                    prev_warning = true;
                    construct.pop_back();
                } else {
                    construct = " " + construct;
                    prev_warning = false;
                    errors += 1;
                }
                printf("%s",construct.c_str());
            }
        } else {
            if (prev_warning) {
                printf("\n");
                prev_warning = false;
            }
            printf("%03d: %04d\n", pair.first, pair.second);
        }
    }
    printf("\n");
    for (const auto& pair : symbol_table) {
        const string& symbol = pair.first;
        if (used_e_instruction.find(symbol) == used_e_instruction.end()) {
            printf("Warning: Module %d: %s was defined but never used\n", symbol_module_number[symbol],symbol.c_str());
            warnings += 1;
        }
    }

    printf("\nSummary: Errors=%d Warnings=%d", errors, warnings);

    fclose(input_file);

    return 0;
}