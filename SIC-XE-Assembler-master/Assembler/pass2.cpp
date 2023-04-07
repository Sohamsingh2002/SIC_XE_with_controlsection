#include "pass1.cpp"
using namespace std;

ifstream intermediatefile;ofstream errorfile,objectfile,Listingfile;ofstream printtab ;string writestring ;bool isComment;
string label,opcode,operand,comment,op_1,op_2;
int line_no,blockNumber,address,startAddress;
string objectCode, wd, currentRecord, modificationRecord="M^", endRecord, write_R_Data, write_D_Data,current_section_name="DEFAULT";
int section_counter=0;int program_section_length=0;int program_counter, base_register_value, currentTextRecordLength;bool nobase;




string createObjectCodeFormat34(){
  string objcode;int halfBytes;
  halfBytes = (get_flag_format(opcode)=='+')?5:3;

  if(get_flag_format(operand)=='#'){
    if(operand.substr(operand.size()-2,2)==",X"){
      wd = "Line: "+to_string(line_no)+" index based addressing not supported with Indirect addressing";
      write_to_file(errorfile,wd);
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }

    string tempoperand = operand.substr(1,operand.size()-1);
    if(ifall_num(tempoperand)||((symb_tab[tempoperand].is_exists)&&(symb_tab[tempoperand].relative==0)
    &&(csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists==false))){
      int immediateValue;

      if(ifall_num(tempoperand)){
        immediateValue = stoi(tempoperand);
      }
      else{
        immediateValue = hexadecimal_string_to_int(symb_tab[tempoperand].address);
      }
     
      if(immediateValue>=(1<<4*halfBytes)){
        wd = "Line: "+to_string(line_no)+" Immediate value exceeds format limit";
        write_to_file(errorfile,wd);
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
        objcode += (halfBytes==5)?"100000":"0000";
      }
      else{
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
        objcode += (halfBytes==5)?'1':'0';
        objcode += int_to_string_hexadecimal(immediateValue,halfBytes);
      }
      return objcode;
    }
    else if(symb_tab[tempoperand].is_exists==false ||csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists) {
     
      if(csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists!=true || halfBytes==3) { 
      wd = "Line "+to_string(line_no);
     if(halfBytes==3 && csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists){
         wd+= " : Invalid format for external reference " + tempoperand; 
      } else{ 
         wd += " : Symbol doesn't exists. Found " + tempoperand;
       } 
      write_to_file(errorfile,wd);
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }
   
            if(symb_tab[tempoperand].is_exists && csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists) {
             objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
             objcode += "100000";

             modificationRecord += "M^" + int_to_string_hexadecimal(address+1,6) + '^';
             modificationRecord += "05+";
             modificationRecord += csect_tab[current_section_name].EXTREF_TAB[tempoperand].name ;
             modificationRecord += '\n';       

            return objcode ;
         }

    }
    else{
      int operandAddress = hexadecimal_string_to_int(symb_tab[tempoperand].address) + hexadecimal_string_to_int(blocks[blocks_num_to_name[symb_tab[tempoperand].blockNumber]].startAddress);

      
      if(halfBytes==5){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
        objcode += '1';
        objcode += int_to_string_hexadecimal(operandAddress,halfBytes);


        modificationRecord += "M^" + int_to_string_hexadecimal(address+1,6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }

  
      program_counter = address + hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress);
      program_counter += (halfBytes==5)?4:3;
      int relativeAddress = operandAddress - program_counter;

  
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
        objcode += '2';
        objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
        return objcode;
      }

   
      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
          objcode += '4';
          objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
          return objcode;
        }
      }

    
      if(operandAddress<=4095){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+1,2);
        objcode += '0';
        objcode += int_to_string_hexadecimal(operandAddress,halfBytes);


        modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
  }
  else if(get_flag_format(operand)=='@'){
    string tempoperand = operand.substr(1,operand.size()-1);
    if(tempoperand.substr(tempoperand.size()-2,2)==",X" || symb_tab[tempoperand].is_exists==false /*****/||csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists==true/*****/){//Error handling for Indirect with ind based
      

      if(csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists!=true || halfBytes==3) {
      wd = "Line "+to_string(line_no);
 if(halfBytes==3 && csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists){
         wd+= " : Invalid format for external reference " + tempoperand; 
      } else{ 
         wd += " : Symbol doesn't exists.ind based addressing not supported with Indirect addressing ";
    } 
      write_to_file(errorfile,wd);
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }

    if(symb_tab[tempoperand].is_exists && csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists) {
          objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
             objcode += "100000";

             modificationRecord += "M^" + int_to_string_hexadecimal(address+1,6) + '^';
             modificationRecord += "05+";
             modificationRecord += csect_tab[current_section_name].EXTREF_TAB[tempoperand].name ;
             modificationRecord += '\n';      

             return objcode ; 
         }
}
    int operandAddress = hexadecimal_string_to_int(symb_tab[tempoperand].address) + hexadecimal_string_to_int(blocks[blocks_num_to_name[symb_tab[tempoperand].blockNumber]].startAddress);
    program_counter = address + hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
        objcode += '2';
        objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
          objcode += '4';
          objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
        objcode += '0';
        objcode += int_to_string_hexadecimal(operandAddress,halfBytes);

   
        modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
      objcode += '1';
      objcode += int_to_string_hexadecimal(operandAddress,halfBytes);

     
      modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    wd = "Line: "+to_string(line_no);
    wd += "Can't fit into program counter based or base register based addressing.";
    write_to_file(errorfile,wd);
    objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+2,2);
    objcode += (halfBytes==5)?"100000":"0000";

    return objcode;
  }
  else if(get_flag_format(operand)=='='){
    string tempoperand = operand.substr(1,operand.size()-1);

    if(tempoperand=="*"){
      tempoperand = "X'" + int_to_string_hexadecimal(address,6) + "'";
    
      modificationRecord += "M^" + int_to_string_hexadecimal(hexadecimal_string_to_int(lit_tab[tempoperand].address)+hexadecimal_string_to_int(blocks[blocks_num_to_name[lit_tab[tempoperand].blockNumber]].startAddress),6) + '^';
      modificationRecord += int_to_string_hexadecimal(6,2);
      modificationRecord += '\n';
    }

    if(lit_tab[tempoperand].is_exists ==false){
      wd = "Line "+to_string(line_no)+" : Symbol doesn't exists. Found " + tempoperand;
      write_to_file(errorfile,wd);

      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
      objcode += (halfBytes==5)?"000":"0";
      objcode += "000";
      return objcode;
    }

    int operandAddress = hexadecimal_string_to_int(lit_tab[tempoperand].address) + hexadecimal_string_to_int(blocks[blocks_num_to_name[lit_tab[tempoperand].blockNumber]].startAddress);
    program_counter = address + hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
        objcode += '2';
        objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
          objcode += '4';
          objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
        objcode += '0';
        objcode += int_to_string_hexadecimal(operandAddress,halfBytes);

      
        modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
      objcode += '1';
      objcode += int_to_string_hexadecimal(operandAddress,halfBytes);

    
      modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    wd = "Line: "+to_string(line_no);
    wd += "Can't fit into program counter based or base register based addressing.";
    write_to_file(errorfile,wd);
    objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
    objcode += (halfBytes==5)?"100":"0";
    objcode += "000";

    return objcode;
  }
  else{
    int xbpe=0;
    string tempoperand = operand;
    if(operand.substr(operand.size()-2,2)==",X"){
      tempoperand = operand.substr(0,operand.size()-2);
      xbpe = 8;
    }


    if(symb_tab[tempoperand].is_exists==false||csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists) {
    
      if(csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists!=true || halfBytes==3) { 


      wd = "Line "+to_string(line_no);
      if(halfBytes==3 && csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists){
         wd+= " : Invalid format for external reference " + tempoperand; 
      } else{ 
         wd += " : Symbol doesn't exists. Found " + tempoperand;
       } 
      write_to_file(errorfile,wd);
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
      objcode += (halfBytes==5)?(int_to_string_hexadecimal(xbpe+1,1)+"00"):int_to_string_hexadecimal(xbpe,1);
      objcode += "000";
      return objcode;
    }

 if(symb_tab[tempoperand].is_exists && csect_tab[current_section_name].EXTREF_TAB[tempoperand].is_exists) {



             objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
             objcode += "100000";


             modificationRecord += "M^" + int_to_string_hexadecimal(address+1,6) + '^';
             modificationRecord += "05+";
             modificationRecord += csect_tab[current_section_name].EXTREF_TAB[tempoperand].name ;
             modificationRecord += '\n';       


            return objcode ;
         }

    }
else{
  

    int operandAddress = hexadecimal_string_to_int(symb_tab[tempoperand].address) + hexadecimal_string_to_int(blocks[blocks_num_to_name[symb_tab[tempoperand].blockNumber]].startAddress);
    program_counter = address + hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
        objcode += int_to_string_hexadecimal(xbpe+2,1);
        objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
          objcode += int_to_string_hexadecimal(xbpe+4,1);
          objcode += int_to_string_hexadecimal(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
        objcode += int_to_string_hexadecimal(xbpe,1);
        objcode += int_to_string_hexadecimal(operandAddress,halfBytes);

 
        modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{
      objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
      objcode += int_to_string_hexadecimal(xbpe+1,1);
      objcode += int_to_string_hexadecimal(operandAddress,halfBytes);

    
      modificationRecord += "M^" + int_to_string_hexadecimal(address+1+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    wd = "Line: "+to_string(line_no);
    wd += "Can't fit into program counter based or base register based addressing.";
    write_to_file(errorfile,wd);
    objcode = int_to_string_hexadecimal(hexadecimal_string_to_int(op_tab[get_original_opcode(opcode)].opcode)+3,2);
    objcode += (halfBytes==5)?(int_to_string_hexadecimal(xbpe+1,1)+"00"):int_to_string_hexadecimal(xbpe,1);
    objcode += "000";

    return objcode;
  }}
}


string read_until_tab(string data,int& ind){
  string temp_buff = "";

  while(ind<data.size()){
    if( data[ind] != '\t'){
    temp_buff += data[ind];
    ind++;
    }
    else{
      break;
    }
  }
  ind++;
  if(temp_buff==" "){
    temp_buff="-1" ;
  }
  return temp_buff;
}


bool read_Intermediate_file(ifstream& readfile,bool& isComment, int& line_no, int& address, int& blockNumber, string& label, string& opcode, string& operand, string& comment){
  string fileLine="",temp_buff="";bool tempStatus;int ind=0;
  if(!getline(readfile, fileLine)){
    return false;
  }
  line_no = stoi(read_until_tab(fileLine,ind));
  isComment = (fileLine[ind]=='.')?true:false;
  if(isComment){
    rd_first_nonwhitespace(fileLine,ind,tempStatus,comment,true);
    return true;
  }
  address = hexadecimal_string_to_int(read_until_tab(fileLine,ind));
  temp_buff = read_until_tab(fileLine,ind);
  if(temp_buff == " "){
    blockNumber = -1;
  }
  else{
    blockNumber = stoi(temp_buff);
  }
  label = read_until_tab(fileLine,ind);
  if(label=="-1"){
    label=" " ;
  }
  opcode = read_until_tab(fileLine,ind);
  if(opcode=="BYTE"){
    rd_byte_operand(fileLine,ind,tempStatus,operand);
  }
  else{
    operand = read_until_tab(fileLine,ind);
    if(operand=="-1"){
      operand=" " ;
    }
    if(opcode=="CSECT"){
      return true ;
    }
  }
  rd_first_nonwhitespace(fileLine,ind,tempStatus,comment,true);  
  return true;
  
}


void writeTextRecord(bool lastRecord=false){
  if(lastRecord){
    if(currentRecord.size()>0){
      wd = int_to_string_hexadecimal(currentRecord.size()/2,2) + '^' + currentRecord;
      write_to_file(objectfile,wd);
      currentRecord = "";
    }
    return;
  }
  if(objectCode != ""){
    if(currentRecord.size()==0){
      wd = "T^" + int_to_string_hexadecimal(address+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
      write_to_file(objectfile,wd,false);
    }
  
    if((currentRecord + objectCode).size()>60){

      wd = int_to_string_hexadecimal(currentRecord.size()/2,2) + '^' + currentRecord;
      write_to_file(objectfile,wd);


      currentRecord = "";
      wd = "T^" + int_to_string_hexadecimal(address+hexadecimal_string_to_int(blocks[blocks_num_to_name[blockNumber]].startAddress),6) + '^';
      write_to_file(objectfile,wd,false);
    }

    currentRecord += objectCode;
  }
  else{
    
    if(opcode=="START"||opcode=="END"||opcode=="BASE"||opcode=="NOBASE"||opcode=="LTORG"||opcode=="ORG"||opcode=="EQU"/****/||opcode=="EXTREF"||opcode=="EXTDEF"/******/){
   
    }
    else{
     
      if(currentRecord.size()>0){
        wd = int_to_string_hexadecimal(currentRecord.size()/2,2) + '^' + currentRecord;
        write_to_file(objectfile,wd);
      }
      currentRecord = "";
    }
  }
}


void writeDRecord(){
    write_D_Data="D^" ;
    string temp_address="" ;
    int length=operand.size() ;
    string inp="" ;
    for(int i=0;i<length;i++){
      while(operand[i]!=',' && i<length){
        inp+=operand[i] ;
        i++ ;
      }
      temp_address=csect_tab[current_section_name].EXTDEF_TAB[inp].address ;
      write_D_Data+=string_expansion(inp,6,' ',true)+temp_address;
      inp="" ;
    }
    write_to_file(objectfile,write_D_Data);
}

void writeRRecord(){
    write_R_Data="R^" ;
    string temp_address="" ;
    int length=operand.size() ;
    string inp="" ;
    for(int i=0;i<length;i++){
      while(operand[i]!=',' && i<length){
        inp+=operand[i] ;
        i++ ;
      }
      write_R_Data+=string_expansion(inp,6,' ',true);
      inp="" ;
    }
    write_to_file(objectfile,write_R_Data);
}


void writeEndRecord(bool write=true){
  if(write){
    if(endRecord.size()>0){
      write_to_file(objectfile,endRecord);
     
    }
    else{
      writeEndRecord(false);
    }
  }
  if((operand==""||operand==" ")&&current_section_name=="DEFAULT"){
    endRecord = "E^" + int_to_string_hexadecimal(startAddress,6);
  }else if(current_section_name!="DEFAULT"){
    endRecord = "E";
  }
  else{
    int firstExecutableAddress;
   
      firstExecutableAddress = hexadecimal_string_to_int(symb_tab[firstexecutable_section].address);
    
    endRecord = "E^" + int_to_string_hexadecimal(firstExecutableAddress,6)+"\n";
  }
  
}

void pass2(){
  string temp_buff;
  intermediatefile.open("Pass1_Outputs\\intermediate.txt");//begin
  if(!intermediatefile){
    cout<<"Can't opem file : intermediate.txt" <<endl;
    exit(1);
  }
  getline(intermediatefile, temp_buff); 

  objectfile.open("Pass2_Outputs\\object.txt");
  if(!objectfile){
    cout<<"Can't open file : object.txt" <<endl;
    exit(1);
  }

  Listingfile.open("Pass2_Outputs\\listing.txt");
  if(!Listingfile){
    cout<<"Can't open file : listing.txt" <<endl;
    exit(1);
  }
  write_to_file(Listingfile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tObjectCode\tComment");

  errorfile.open("Error_Output\\errors.txt",fstream::app);
  if(!errorfile){
    cout<<"Can't open file : errors.txt"<<endl;
    exit(1);
  }
  write_to_file(errorfile,"\n\nPASS2_ERRORS------------>");

  objectCode = ""; currentTextRecordLength=0; currentRecord = ""; modificationRecord = "";blockNumber = 0;nobase = true;

  read_Intermediate_file(intermediatefile,isComment,line_no,address,blockNumber,label,opcode,operand,comment);
  while(isComment){
    wd = to_string(line_no) + "\t" + comment;
    write_to_file(Listingfile,wd);
    read_Intermediate_file(intermediatefile,isComment,line_no,address,blockNumber,label,opcode,operand,comment);
  }

  if(opcode=="START"){
    startAddress = address;
    wd = to_string(line_no) + "\t" + int_to_string_hexadecimal(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
    write_to_file(Listingfile,wd);
  }
  else{
    label = "";
    startAddress = 0;
    address = 0;
  }

  if(blocks.size()>1){
    program_section_length = program_len ;
  }
  else{
    program_section_length=csect_tab[current_section_name].length ;
  }


  wd = "H^"+string_expansion(label,6,' ',true)+'^'+int_to_string_hexadecimal(address,6)+'^'+int_to_string_hexadecimal(program_section_length,6);
  write_to_file(objectfile,wd);
 
  read_Intermediate_file(intermediatefile,isComment,line_no,address,blockNumber,label,opcode,operand,comment);
  currentTextRecordLength  = 0;



  while(opcode!="END"){
      while(opcode!="END" && opcode!="CSECT"){
    if(!isComment){
      if(op_tab[get_original_opcode(opcode)].is_exists){
        if(op_tab[get_original_opcode(opcode)].format_no==1){
          objectCode = op_tab[get_original_opcode(opcode)].opcode;
        }
        else if(op_tab[get_original_opcode(opcode)].format_no==2){
          op_1 = operand.substr(0,operand.find(','));
          op_2 = operand.substr(operand.find(',')+1,operand.size()-operand.find(',') -1 );

          if(op_2==operand){
            if(get_original_opcode(opcode)=="SVC"){
              objectCode = op_tab[get_original_opcode(opcode)].opcode + int_to_string_hexadecimal(stoi(op_1),1) + '0';
            }
            else if(reg_tab[op_1].is_exists){
              objectCode = op_tab[get_original_opcode(opcode)].opcode + reg_tab[op_1].num + '0';
            }
            else{
              objectCode = get_original_opcode(opcode) + '0' + '0';
              wd = "Line: "+to_string(line_no)+" Invalid Register name";
              write_to_file(errorfile,wd);
            }
          }
          else{
            if(reg_tab[op_1].is_exists==false){
              objectCode = op_tab[get_original_opcode(opcode)].opcode + "00";
              wd = "Line: "+to_string(line_no)+" Invalid Register name";
              write_to_file(errorfile,wd);
            }
            else if(get_original_opcode(opcode)=="SHIFTR" || get_original_opcode(opcode)=="SHIFTL"){
              objectCode = op_tab[get_original_opcode(opcode)].opcode + reg_tab[op_1].num + int_to_string_hexadecimal(stoi(op_2),1);
            }
            else if(reg_tab[op_2].is_exists==false){
              objectCode = op_tab[get_original_opcode(opcode)].opcode + "00";
              wd = "Line: "+to_string(line_no)+" Invalid Register name";
              write_to_file(errorfile,wd);
            }
            else{
              objectCode = op_tab[get_original_opcode(opcode)].opcode + reg_tab[op_1].num + reg_tab[op_2].num;
            }
          }
        }
        else if(op_tab[get_original_opcode(opcode)].format_no==3){
          if(get_original_opcode(opcode)=="RSUB"){
            objectCode = op_tab[get_original_opcode(opcode)].opcode;
            objectCode += (get_flag_format(opcode)=='+')?"000000":"0000";
          }
          else{
            objectCode = createObjectCodeFormat34();
          }
        }
      }
      else if(opcode=="BYTE"){
        if(operand[0]=='X'){
          objectCode = operand.substr(2,operand.size()-3);
        }
        else if(operand[0]=='C'){
          objectCode = stringToHexString(operand.substr(2,operand.size()-3));
        }
      }
      else if(label=="*"){
        if(opcode[1]=='C'){
          objectCode = stringToHexString(opcode.substr(3,opcode.size()-4));
        }
        else if(opcode[1]=='X'){
          objectCode = opcode.substr(3,opcode.size()-4);
        }
      }
      else if(opcode=="WORD"){
        objectCode = int_to_string_hexadecimal(stoi(operand),6);
      }
      else if(opcode=="BASE"){
        if(symb_tab[operand].is_exists){
          base_register_value = hexadecimal_string_to_int(symb_tab[operand].address) + hexadecimal_string_to_int(blocks[blocks_num_to_name[symb_tab[operand].blockNumber]].startAddress);
          nobase = false;
        }
        else{
          wd = "Line "+to_string(line_no)+" : Symbol doesn't exists. Found " + operand;
          write_to_file(errorfile,wd);
        }
        objectCode = "";
      }
      else if(opcode=="NOBASE"){
        if(nobase){
          wd = "Line "+to_string(line_no)+": Assembler wasn't using base addressing";
          write_to_file(errorfile,wd);
        }
        else{
          nobase = true;
        }
        objectCode = "";
      }
      else{
        objectCode = "";
      }
      
      writeTextRecord();

      if(blockNumber==-1 && address!=-1){
        wd = to_string(line_no) + "\t" + int_to_string_hexadecimal(address) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
      }
      else if(address==-1){
        if(opcode=="EXTDEF"){
          writeDRecord() ;
        } else if(opcode=="EXTREF"){
          writeRRecord() ;
        }
        wd = to_string(line_no) + "\t" + " " + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
      } 
     
      else{ wd = to_string(line_no) + "\t" + int_to_string_hexadecimal(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
      }
    }
    else{
      wd = to_string(line_no) + "\t" + comment;
    }
    write_to_file(Listingfile,wd);
    read_Intermediate_file(intermediatefile,isComment,line_no,address,blockNumber,label,opcode,operand,comment);//Read next line
    objectCode = "";
  }
 
    


  writeTextRecord();

 
  writeEndRecord(false);
  
   if(opcode=="CSECT"&&!isComment){
        wd = to_string(line_no) + "\t" + int_to_string_hexadecimal(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + opcode + "\t" + " " + "\t" + objectCode +"\t"+" ";
      }else if(!isComment){
  wd = to_string(line_no) + "\t" + int_to_string_hexadecimal(address) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + "" +"\t" + comment;
}else{
  wd = to_string(line_no) + "\t" + comment;
 }
  write_to_file(Listingfile,wd);

if(opcode!="CSECT"){
  while(read_Intermediate_file(intermediatefile,isComment,line_no,address,blockNumber,label,opcode,operand,comment)){
    if(label=="*"){
      if(opcode[1]=='C'){
        objectCode = stringToHexString(opcode.substr(3,opcode.size()-4));
      }
      else if(opcode[1]=='X'){
        objectCode = opcode.substr(3,opcode.size()-4);
      }
      writeTextRecord();
    }
    wd = to_string(line_no) + "\t" + int_to_string_hexadecimal(address) + "\t" + to_string(blockNumber) + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
    write_to_file(Listingfile,wd);
  }
}   
  
writeTextRecord(true);
if(!isComment){
  
  write_to_file(objectfile,modificationRecord,false);
  writeEndRecord(true);
  current_section_name=label;
  modificationRecord="";
}
if(!isComment&&opcode!="END"){
wd = "\n********************object program for "+ label+" **************";
  write_to_file(objectfile,wd);
  


wd = "\nH^"+string_expansion(label,6,' ',true)+'^'+int_to_string_hexadecimal(address,6)+'^'+int_to_string_hexadecimal(csect_tab[label].length,6);
  write_to_file(objectfile,wd);
  }
 
  read_Intermediate_file(intermediatefile,isComment,line_no,address,blockNumber,label,opcode,operand,comment);//Read next line
    objectCode = "";

}

}



int main(){
  load_tables();
  cout<<"Performing PASS1 ... "<<endl;
 
  pass1();

  printtab.open("Pass1_Outputs\\tables.txt") ;
  write_to_file(printtab,"SYMBOL TABLE ------------> \n") ;
    for (auto  it= symb_tab.begin();it!=symb_tab.end();it++) { 
        writestring+=it->first+":-\t"+ "name:"+it->second.name+"\t|"+ "address:"+it->second.address+"\t|"+ "relative:"+int_to_string_hexadecimal(it->second.relative)+" \n" ;
    } 
    write_to_file(printtab,writestring) ;
    writestring="" ;
  
  
  write_to_file(printtab,"LITERAL TABLE ------------> \n") ;
    for (auto it= lit_tab.begin();it!=lit_tab.end();it++) { 
        writestring+=it->first+":-\t"+ "value:"+it->second.value+"\t|"+ "address:"+it->second.address+" \n" ;
    } 
    write_to_file(printtab,writestring) ;
    writestring="" ;


  write_to_file(printtab,"EXTREF TABLE ------------> \n") ;
    for (auto  it0= csect_tab.begin();it0!=csect_tab.end();it0++) { 
      for (auto  it= it0->second.EXTREF_TAB.begin();it!=it0->second.EXTREF_TAB.end();it++) 
        writestring+=it->first+":-\t"+ "name:"+it->second.name+"\t|"+it0->second.name+" \n" ;
    
    } 
    write_to_file(printtab,writestring) ;
    writestring="" ;
   
 
  write_to_file(printtab,"EXTDEF TABLE ------------> \n") ;
    for (auto  it0= csect_tab.begin();it0!=csect_tab.end();it0++) { 
      for (auto  it= it0->second.EXTDEF_TAB.begin();it!=it0->second.EXTDEF_TAB.end();it++) {
        if(it->second.name!="undefined")
          writestring+=it->first+":-\t"+ "name:"+it->second.name+"\t|"+ "address:"+it->second.address+"\t|"+" \n" ;
      }
    }
    
    write_to_file(printtab,writestring) ;
    writestring="";

  cout<<"Performing PASS2 ... "<<endl;
 
  pass2();

  cout << "PROGRAM EXECUTION COMPLETED";


}
