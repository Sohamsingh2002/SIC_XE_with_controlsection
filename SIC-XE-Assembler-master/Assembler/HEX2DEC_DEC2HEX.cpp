#include <bits/stdc++.h>
using namespace std;

bool is_white_space(char a){
  if(a == ' ' || a == '\t'){
    return true;
  }
  return false;
}

bool is_comment_line(string line){
  if(line[0]=='.'){
    return true;
  }
  return false;
}


bool ifall_num(string x){
  bool all = true;
  int i = 0;
  while(all && (i<x.size())){
    all &= isdigit(x[i++]);
  }
  return all;
}

string string_expansion(string data,int length,char f_char,bool b=false){
  if(b){
    if(length<=data.size()){
      return data.substr(0,length);
    }
    else{
      for(int i = length-data.size();i>0;i--){
        data += f_char;
      }
    }
  }
  else{
    if(length<=data.size()){
      return data.substr(data.size()-length,length);
    }
    else{
      for(int i = length-data.size();i>0;i--){
        data = f_char + data;
      }
    }
  }
  return data;
}



int hexadecimal_string_to_int(string x){
  return stoul(x,nullptr,16);
}

string stringToHexString(const string& input){
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.size();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}


string int_to_string_hexadecimal(int x,int f = 5){
  stringstream s;
  s << setfill('0') << setw(f) << hex << x;
  string temp = s.str();
  temp = temp.substr(temp.size()-f,f);
  transform(temp.begin(), temp.end(),temp.begin(),::toupper);
  return temp;
}




void rd_first_nonwhitespace(string line,int& ind,bool& status,string& data,bool readTillEnd=false){
  data = "";
  status = true;
  if(readTillEnd){
    data = line.substr(ind,line.size() - ind);
    if(data==""){
      status = false;
    }
    return;
  }
  while(ind<line.size() && !is_white_space(line[ind])){
    data += line[ind];
    ind++;
  }

  if(data==""){
    status = false;
  }

  while(ind<line.size()&& is_white_space(line[ind])){
    ind++;
  }
}

void rd_byte_operand(string line,int& ind,bool& status,string& data){
  data = "";
  status = true;
  if(line[ind]=='C'){
    data += line[ind++];
    char identifier = line[ind++];
    data += identifier;
    while(ind<line.size() && line[ind]!=identifier){
      data += line[ind];
      ind++;
    }
    data += identifier;
    ind++;
  }
  else{
    while(ind<line.size()&&!is_white_space(line[ind])){
      data += line[ind];
      ind++;
    }
  }

  if(data==""){
    status = false;
  }

  while(ind<line.size()&&is_white_space(line[ind])){
    ind++;
  }
}

void write_to_file(ofstream& file,string data,bool newline=true){
  if(newline){
    file<<data<<endl;
  }else{
    file<<data;
  }
}

string get_original_opcode(string opcode){
  if(opcode[0] == '+' || opcode[0] == '@'){
    return opcode.substr(1,opcode.size() - 1);
  }
  return opcode;
}

char get_flag_format(string data){
  if(data[0] == '#' || data[0] == '+' || data[0] == '@' || data[0] == '='){
    return data[0];
  }
  return ' ';
}

class Eva_Str{
  public:
    string stored_data;
    int ind;


    Eva_Str(string data){
      stored_data = data;
      ind=0;
    }

    int getResult(){
      int result = term();
      while(peek()=='+' || peek() == '-'){
        if(get() == '+'){
        result += term();
        }else{
        result -= term();
        }
      }
      return result;
    }

  
    char peek(){
      return stored_data[ind];
    }
    char get(){
      return stored_data[ind++];
    }

    int factor(){
      if(peek() >= '0' && peek() <= '9'){
        return number();
       }
    else if(peek() == '('){
      get();
      int result = getResult();
      get();
      return result;
    }
    else if(peek()=='-'){
      get();
    return -factor();
    }
    return 0;
    }

  int number(){
      int result = get() - '0';
      while(peek() >= '0' && peek() <= '9'){
        result = 10*result + get()-'0';
      }
      return result;
  }

  int term(){
      int result = factor();
      while(peek() == '*' || peek() == '/'){
        if(get()=='*'){
          result *= factor();
        }
         else{
          result /= factor();
        }
      }
      return result;
    }
};







