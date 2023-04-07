#include <bits/stdc++.h>
#include "HEX2DEC_DEC2HEX.cpp"
#include "all_tables.cpp"
using namespace std;

//global variables

string filename="input.txt";
bool e_flag=false; int program_len; string *blocks_num_to_name; string firstexecutable_section;


void expression_evaluation(string expression, bool& relative,string& tempoperand,int lineNumber, ofstream& errorfile,bool& e_flag){
  string single_operand="?" , single_operator="?", value_str="", value_temp="", wd="";
  int last_operand=0 , last_operator=0 , paircount=0 ; char last_byte = ' ' ; bool illegal = false;

  for(int i=0;i<expression.size();){

    single_operand = "";
    last_byte = expression[i];
  
    while((last_byte!='+' && last_byte!='-' && last_byte!='/' && last_byte!='*') ){
      if(i<expression.size()){
      single_operand = single_operand + last_byte;
      last_byte = expression[++i];
      }
    }

    if(symb_tab[single_operand].is_exists){
      last_operand = symb_tab[single_operand].relative;
      value_temp = to_string(hexadecimal_string_to_int(symb_tab[single_operand].address));
    }
    else if((single_operand != "" || single_operand !="?" ) ){
      if(ifall_num(single_operand)){
      last_operand = 0;
      value_temp = single_operand;
      }
    }
    else{
      wd = "Line: "+to_string(lineNumber)+" : Can't find symbol. Found "+single_operand;
      write_to_file(errorfile,wd);
      illegal = true;
      break;
    }

    if(last_operand*last_operator == 1){
      wd = "Line: "+to_string(lineNumber)+" : illegal expression";
      write_to_file(errorfile,wd);
      e_flag = true;
      illegal = true;
      break;
    }
    else if((single_operator=="-" || single_operator=="+" || single_operator=="?")){
      if(last_operand==1){
      if(single_operator=="-"){
        paircount--;
      }
      else{
        paircount++;
      }
      }
    }

    value_str += value_temp;

    single_operator= "";
    while(i<expression.size() && (last_byte=='+'||last_byte=='-'||last_byte=='/'||last_byte=='*')){
      single_operator += last_byte;
      last_byte = expression[++i];
    }

    if(single_operator.size()>1){
      wd = "Line: "+to_string(lineNumber)+" : illegal operator in expression. Found "+single_operator;
      write_to_file(errorfile,wd);
      e_flag = true;
      illegal = true;
      break;
    }

    if(single_operator=="*" || single_operator == "/"){
      last_operator = 1;
    }
    else{
      last_operator = 0;
    }

    value_str += single_operator;
  }

  if(!illegal){
    if(paircount==1){
 
      relative = 1;
      Eva_Str tempobj(value_str);
      tempoperand = int_to_string_hexadecimal(tempobj.getResult());
    }
    else if(paircount==0){
   
      relative = 0;
    
      Eva_Str tempobj(value_str);
      tempoperand = int_to_string_hexadecimal(tempobj.getResult());
    }
    else{
      wd = "Line: "+to_string(lineNumber)+" : illegal expression";
      write_to_file(errorfile,wd);
      e_flag = true;
      tempoperand = "00000";
      relative = 0;
    }
  }
  else{
    tempoperand = "00000";
    e_flag = true;
    relative = 0;
  }
}



void handle_ltorg(string& litPrefix, int& lineNumberDelta,int lineNumber,int& LOCCTR, int& lastDeltaLOCCTR, int currentBlockNumber){
  string lit_address , lit_value ; litPrefix = "";
  for(auto it = lit_tab.begin();it!=lit_tab.end();it++){
    lit_address = (it->second).address;
    lit_value = (it->second).value;
  
  if(lit_address=="?"){
     lineNumberDelta += 5;
      lineNumber += 5;
     
      lit_tab[it->first].address = int_to_string_hexadecimal(LOCCTR);
      lit_tab[it->first].blockNumber = currentBlockNumber;
      litPrefix += "\n" + to_string(lineNumber) + "\t" + int_to_string_hexadecimal(LOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + "*" + "\t" + "="+lit_value + "\t" + " " + "\t" + " ";

      if(lit_value[0]=='X'){
        LOCCTR += (lit_value.size() -3)/2;
        lastDeltaLOCCTR += (lit_value.size() -3)/2;
      }
      else if(lit_value[0]=='C'){
        LOCCTR += (lit_value.size() -3);
        lastDeltaLOCCTR += (lit_value.size() -3);
      }
    }
  }
}



void pass1(){
  ifstream sourcefile;
  ofstream intermediatefile, errorfile;

  sourcefile.open(filename);

  if(!sourcefile){

    cout << "Can't open file :" << filename << endl;

    exit(1);
  }

  intermediatefile.open("Pass1_Outputs\\intermediate.txt");

  if(!intermediatefile){

    cout <<  "Can't open file : intermediate.txt" << endl;

    exit(1);
  }

  write_to_file(intermediatefile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");

  errorfile.open("Error_Output\\errors.txt");
  if(!errorfile){

    cout << "Can't open file: errors.txt "<<endl;

    exit(1);
  }

  write_to_file(errorfile,"PASS1_ERRORS------------>");  

  string fileLine;string wd,wdSuffix="",wdPrefix="";int ind=0;
  string currentBlockName = "DEFAULT";int currentBlockNumber = 0;int totalblocks = 1;
  bool statusCode;string label,opcode,operand,comment;string tempoperand;
  int startAddress,LOCCTR,saveLOCCTR,lineNumber,lastDeltaLOCCTR,lineNumberDelta=0;lineNumber = 0;lastDeltaLOCCTR = 0;
  getline(sourcefile,fileLine);
  lineNumber += 5;
  while(is_comment_line(fileLine)){
    wd = to_string(lineNumber) + "\t" + fileLine;
    write_to_file(intermediatefile,wd);
    getline(sourcefile,fileLine);
    lineNumber += 5;
    ind = 0;
  }

  rd_first_nonwhitespace(fileLine,ind,statusCode,label);
  rd_first_nonwhitespace(fileLine,ind,statusCode,opcode);


  if(opcode=="START"){
    rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
    rd_first_nonwhitespace(fileLine,ind,statusCode,comment,true);
    startAddress = hexadecimal_string_to_int(operand);
 
    LOCCTR = startAddress;
    wd = to_string(lineNumber) + "\t" + int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    write_to_file(intermediatefile,wd); 

    getline(sourcefile,fileLine); 
    lineNumber += 5;
    ind = 0;
    rd_first_nonwhitespace(fileLine,ind,statusCode,label); 
    rd_first_nonwhitespace(fileLine,ind,statusCode,opcode);
  }
  else{
    startAddress = 0;
    LOCCTR = 0;
  }

   string current_section_name="DEFAULT" ;
   int section_counter=0;
  
  while(opcode!="END"){

  	while(opcode!="END" && opcode!="CSECT"){

    if(!is_comment_line(fileLine)){

      if(label!=""){
        if(!symb_tab[label].is_exists){
          labels_information obj(label,int_to_string_hexadecimal(LOCCTR),currentBlockNumber,true,1);
          symb_tab[label]=obj;

  
 		  if(csect_tab[current_section_name].EXTDEF_TAB[label].is_exists){
 		  		csect_tab[current_section_name].EXTDEF_TAB[label].address=symb_tab[label].address ;
 		  }
 
        }
        else{
          wd = "Line: "+to_string(lineNumber)+" : Duplicate symbol for '"+label+"'. Previously defined at "+symb_tab[label].address;
          write_to_file(errorfile,wd);
          e_flag = true;
        }
      }
      if(op_tab[get_original_opcode(opcode)].is_exists){
        if(op_tab[get_original_opcode(opcode)].format_no==3){
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
          if(get_flag_format(opcode)=='+'){
            LOCCTR += 1;
            lastDeltaLOCCTR += 1;
          }
          if(get_original_opcode(opcode)=="RSUB"){
            operand = " ";
          }
          else{
            rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
            if(operand[operand.size()-1] == ','){
              rd_first_nonwhitespace(fileLine,ind,statusCode,tempoperand);
              operand += tempoperand;
            }
          }

          if(get_flag_format(operand)=='='){
            tempoperand = operand.substr(1,operand.size()-1);
            if(tempoperand=="*"){
              tempoperand = "X'" + int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR,6) + "'";
            }
            if(!lit_tab[tempoperand].is_exists){
              literal obj(tempoperand,true,"?",-1);
              lit_tab[tempoperand]=obj;
            }
          }
        }
        else if(op_tab[get_original_opcode(opcode)].format_no==1){
          operand = " ";
          LOCCTR += op_tab[get_original_opcode(opcode)].format_no;
          lastDeltaLOCCTR += op_tab[get_original_opcode(opcode)].format_no;
        }
        else{
          LOCCTR += op_tab[get_original_opcode(opcode)].format_no;
          lastDeltaLOCCTR += op_tab[get_original_opcode(opcode)].format_no;
          rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
          if(operand[operand.size()-1] == ','){
            rd_first_nonwhitespace(fileLine,ind,statusCode,tempoperand);
            operand += tempoperand;
          }
        }
      }
     
      else if(opcode == "EXTDEF"){

      	rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
		int length=operand.size() ;
		string inp="" ;
		for(int i=0;i<length;i++){
			while(operand[i]!=',' && i<length){
				inp+=operand[i] ;
				i++ ;
			}
			csect_tab[current_section_name].EXTDEF_TAB[inp].name=inp ;
			csect_tab[current_section_name].EXTDEF_TAB[inp].is_exists=true ;
			inp="" ;
		}
      }
      else if(opcode == "EXTREF"){

      	rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
		int length=operand.size() ;
		string inp="" ;
		for(int i=0;i<length;i++){
			while(operand[i]!=',' && i<length){
				inp+=operand[i] ;
				i++ ;
			}
			csect_tab[current_section_name].EXTREF_TAB[inp].name=inp ;
			csect_tab[current_section_name].EXTREF_TAB[inp].is_exists=true ;
			inp="" ;
		}
      }
   
      else if(opcode == "WORD"){
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
        LOCCTR += 3;
        lastDeltaLOCCTR += 3;
      }
      else if(opcode == "RESW"){
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
        LOCCTR += 3*stoi(operand);
        lastDeltaLOCCTR += 3*stoi(operand);
      }
      else if(opcode == "RESB"){
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
        LOCCTR += stoi(operand);
        lastDeltaLOCCTR += stoi(operand);
      }
      else if(opcode == "BYTE"){
        rd_byte_operand(fileLine,ind,statusCode,operand);
        if(operand[0]=='X'){
          LOCCTR += (operand.size() -3)/2;
          lastDeltaLOCCTR += (operand.size() -3)/2;
        }
        else if(operand[0]=='C'){
          LOCCTR += operand.size() -3;
          lastDeltaLOCCTR += operand.size() -3;
        }
       
      }
      else if(opcode=="BASE"){
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
      }
      else if(opcode=="LTORG"){
        operand = " ";
        handle_ltorg(wdSuffix,lineNumberDelta,lineNumber,LOCCTR,lastDeltaLOCCTR,currentBlockNumber);
      }
      else if(opcode=="ORG"){
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);

        char last_byte = operand[operand.size()-1];
        while(last_byte=='+'||last_byte=='-'||last_byte=='/'||last_byte=='*'){
          rd_first_nonwhitespace(fileLine,ind,statusCode,tempoperand);
          operand += tempoperand;
          last_byte = operand[operand.size()-1];
        }

        int tempVariable;
        tempVariable = saveLOCCTR;
        saveLOCCTR = LOCCTR;
        LOCCTR = tempVariable;

        if(symb_tab[operand].is_exists){
          LOCCTR = hexadecimal_string_to_int(symb_tab[operand].address);
        }
        else{
          bool relative;
      
          expression_evaluation(operand,relative,tempoperand,lineNumber,errorfile,e_flag);
          if(!e_flag){
            LOCCTR = hexadecimal_string_to_int(tempoperand);
          }
          e_flag = false;//reset e_flag
        }
      }
      else if(opcode=="USE"){
        

        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
        blocks[currentBlockName].LOCCTR = int_to_string_hexadecimal(LOCCTR);

        if(blocks[operand].is_exists==false){
        
          blocks[operand].is_exists=true;
          blocks[operand].name = operand;
          blocks[operand].number = totalblocks++;
          blocks[operand].LOCCTR = "0";
        }

       

        currentBlockNumber = blocks[operand].number;
        currentBlockName = blocks[operand].name;
        LOCCTR = hexadecimal_string_to_int(blocks[operand].LOCCTR);

    
      }
      else if(opcode=="EQU"){
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
        tempoperand = "";
        bool relative;

        if(operand=="*"){
          tempoperand = int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR,6);
          relative = 1;
        }
        else if(ifall_num(operand)){
          tempoperand = int_to_string_hexadecimal(stoi(operand),6);
          relative = 0;
        }
        else{
          char last_byte = operand[operand.size()-1];
        
          while(last_byte=='+'||last_byte=='-'||last_byte=='/'||last_byte=='*'){
            rd_first_nonwhitespace(fileLine,ind,statusCode,tempoperand);
            operand += tempoperand;
            last_byte = operand[operand.size()-1];
           

          }
		
          
          expression_evaluation(operand,relative,tempoperand,lineNumber,errorfile,e_flag);
        }

        symb_tab[label].name = label;
        symb_tab[label].address = tempoperand;
        symb_tab[label].relative = relative;
        symb_tab[label].blockNumber = currentBlockNumber;
        lastDeltaLOCCTR = LOCCTR - hexadecimal_string_to_int(tempoperand);
      }
      else{
        rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
        wd = "Line: "+to_string(lineNumber)+" : Invalid OPCODE. Found " + opcode;
        write_to_file(errorfile,wd);
        e_flag = true;
      }
      rd_first_nonwhitespace(fileLine,ind,statusCode,comment,true);
      if(opcode=="EQU" && symb_tab[label].relative == 0){
        wd = wdPrefix + to_string(lineNumber) + "\t" + int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + wdSuffix;
      } 

      else if(opcode=="EXTDEF" || opcode=="EXTREF"){
        wd = wdPrefix + to_string(lineNumber) + "\t"+" " +"\t" +" "+"\t"+" "+ "\t" + opcode + "\t" + operand + "\t" + comment + wdSuffix;    
       }else if(opcode=="CSECT"){
          wd = wdPrefix + to_string(lineNumber) + "\t"+int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR)+"\t" +" "+"\t"+label+ "\t" + opcode + "\t" + " "+ "\t"+" " + wdSuffix;
    }
      else{
        wd = wdPrefix + to_string(lineNumber) + "\t" + int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + wdSuffix;

      }
      wdPrefix = "";
      wdSuffix = "";
    }
    else{
      wd = to_string(lineNumber) + "\t" + fileLine;
    }
    write_to_file(intermediatefile,wd);

    blocks[currentBlockName].LOCCTR = int_to_string_hexadecimal(LOCCTR);
    getline(sourcefile,fileLine); 
    lineNumber += 5 + lineNumberDelta;
    lineNumberDelta = 0;
    ind = 0;
    lastDeltaLOCCTR = 0;
    rd_first_nonwhitespace(fileLine,ind,statusCode,label); 
    rd_first_nonwhitespace(fileLine,ind,statusCode,opcode);
  }

  	
  if(opcode!="END"){

     if(!symb_tab[label].is_exists){
         labels_information obj(label,int_to_string_hexadecimal(LOCCTR),currentBlockNumber,true,1);
          symb_tab[label]=obj;
         
         }

  	csect_tab[current_section_name].LOCCTR=int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR,6) ;
  	csect_tab[current_section_name].length=(LOCCTR-lastDeltaLOCCTR) ;
  	LOCCTR=lastDeltaLOCCTR=0;
  	current_section_name=label;
  	csect_tab[current_section_name].name=current_section_name ;
  	section_counter++;
  	csect_tab[current_section_name].section_number=section_counter ;

  	write_to_file(intermediatefile, wdPrefix + to_string(lineNumber) + "\t" + int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + label + "\t" + opcode);

	getline(sourcefile,fileLine); 
    	lineNumber += 5;

    rd_first_nonwhitespace(fileLine,ind,statusCode,label); 
    rd_first_nonwhitespace(fileLine,ind,statusCode,opcode);
  	
  }
  else{
    csect obj(current_section_name,int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR,6),section_counter,(LOCCTR-lastDeltaLOCCTR));
  	csect_tab[current_section_name]=obj;
  }

}

if(opcode=="END"){
	firstexecutable_section=symb_tab[label].address;
	symb_tab[firstexecutable_section].name=label;
	symb_tab[firstexecutable_section].address=firstexecutable_section;
}
  rd_first_nonwhitespace(fileLine,ind,statusCode,operand);
  rd_first_nonwhitespace(fileLine,ind,statusCode,comment,true);
  currentBlockName = "DEFAULT";
  currentBlockNumber = 0;
  LOCCTR = hexadecimal_string_to_int(blocks[currentBlockName].LOCCTR);
  handle_ltorg(wdSuffix,lineNumberDelta,lineNumber,LOCCTR,lastDeltaLOCCTR,currentBlockNumber);
  wd = to_string(lineNumber) + "\t" + int_to_string_hexadecimal(LOCCTR-lastDeltaLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + wdSuffix;
  write_to_file(intermediatefile,wd);
  int LocctrArr[totalblocks];
  blocks_num_to_name = new string[totalblocks];
  for(auto it= blocks.begin();it!=blocks.end();it++){
    LocctrArr[it->second.number] = hexadecimal_string_to_int(it->second.LOCCTR);
    blocks_num_to_name[it->second.number] = it->first;
  }
  int i=1;
  while(i<totalblocks){
        LocctrArr[i] += LocctrArr[i-1];
        i++;
  }
  

  for(auto it= blocks.begin();it!=blocks.end();it++){
    if(it->second.startAddress=="?"){
      blocks[it->first].startAddress= int_to_string_hexadecimal(LocctrArr[it->second.number - 1]);
    }
  }

  program_len = LocctrArr[totalblocks - 1] - startAddress;

  sourcefile.close();
  intermediatefile.close();
  errorfile.close();
}


