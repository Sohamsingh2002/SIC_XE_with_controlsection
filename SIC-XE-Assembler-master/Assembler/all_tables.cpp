#include <bits/stdc++.h>
using namespace std;

class opcode_information{
  public:
    string opcode; int format_no; bool is_exists;
    opcode_information(){
      opcode="undefined";
      format_no=0;
      is_exists=false;
    }
    opcode_information(string opc,int number,bool exists){
      opcode=opc;format_no=number,is_exists=exists;
    }
};

class labels_information{
  public:
     string address;string name;int relative;int blockNumber; bool is_exists;
     labels_information(){
       name="undefined";
       address="0";
       blockNumber = 0;
       is_exists=false;
       relative = 0;
     }
     labels_information(string Name,string Address,int blockno,bool exists,int rel){
       name=Name;address=Address;blockNumber=blockno;is_exists=exists;relative=rel;
     }
};

class blocks_information{
  public:
     string startAddress; string name; string LOCCTR; int number; bool is_exists;
     blocks_information(){
       name="undefined";
       startAddress="?";
       is_exists=false;
       number = -1;
       LOCCTR = "0";
     }
     blocks_information(bool exists,string nam,string address,int no,string locctr){
       is_exists=exists;name=nam;startAddress=address;number=no;LOCCTR=locctr;
     }
};

class registers_information{
  public:
     char num;bool is_exists;
     registers_information(){
       num = 'F';
       is_exists=false;
     }
     registers_information(char a,bool exist){
       num=a;
       is_exists=exist;
     }
};

class extdef{
  public:
    string name ; string address ; bool is_exists ;
    extdef(){
        name="undefined" ;
        address="0" ;
        is_exists=false ;
    }
};

class extref{
  public:
    string name ; string address ; bool is_exists ;
    extref(){
        name="undefined" ;
        address="0" ;
        is_exists=false ;
    }
};

class csect{
    public:
    string name ; string LOCCTR ;int section_number ; int length ; unordered_map<string,extdef> EXTDEF_TAB ;  unordered_map<string,extref> EXTREF_TAB ;
    csect(){
        name="DEFAULT" ;
        LOCCTR="0" ;
        section_number=0 ;
        length=0 ;
    }
    csect(string Name,string locctr,int sec_no,int len){
      name=Name;locctr=LOCCTR;section_number=sec_no;length=len;
    }
};

class literal{
  public:
    string value; string address;bool is_exists;int blockNumber;
    literal(){
      value="";
      address="?";
      blockNumber = 0;
      is_exists=false;
    }
    literal(string val,bool exists,string Address,int blockno){
      value=val;address=Address;is_exists=exists;blockNumber=blockno;
    }

};


unordered_map<string,labels_information> symb_tab;
unordered_map<string,opcode_information> op_tab;
unordered_map<string,registers_information> reg_tab;
unordered_map<string,literal> lit_tab;
unordered_map<string,blocks_information> blocks;
unordered_map<string,csect> csect_tab;


void ld_op_tab(){
    opcode_information obj_add("18",3,true);
    op_tab["ADD"]=obj_add;
    opcode_information obj_addf("58",3,true);
    op_tab["ADDF"]=obj_addf;
    opcode_information obj_addr("90",2,true);
    op_tab["ADDR"]=obj_addr;
    opcode_information obj_and("40",3,true);
    op_tab["AND"]=obj_and;
    opcode_information obj_clear("B4",2,true);
    op_tab["CLEAR"]=obj_clear;
    opcode_information obj_comp("28",3,true);
    op_tab["COMP"]=obj_comp;
    opcode_information obj_compf("88",3,true);
    op_tab["COMPF"]=obj_compf;
    opcode_information obj_compr("A0",2,true);
    op_tab["COMPR"]=obj_compr;
    opcode_information obj_div("24",3,true);
    op_tab["DIV"]=obj_div;
    opcode_information obj_divf("64",3,true);
    op_tab["DIVF"]=obj_divf;
    opcode_information obj_divr("9C",2,true);
    op_tab["DIVR"]=obj_divr;
    opcode_information obj_fix("C4",1,true);
    op_tab["FIX"]=obj_fix;
    opcode_information obj_float("C0",1,true);
    op_tab["FLOAT"]=obj_float;
    opcode_information obj_hio("F4",1,true);
    op_tab["HIO"]=obj_hio;
    opcode_information obj_jump("3C",3,true);
    op_tab["J"]=obj_jump;
    opcode_information obj_jumpifequal("30",3,true);
    op_tab["JEQ"]=obj_jumpifequal;
    opcode_information obj_jumpifgreater("34",3,true);
    op_tab["JGT"]=obj_jumpifgreater;
    opcode_information obj_jumpifless("38",3,true);
    op_tab["JLT"]=obj_jumpifless;
    opcode_information obj_jsub("48",3,true);
    op_tab["JSUB"]=obj_jsub;
    opcode_information obj_lda("00",3,true);
    op_tab["LDA"]=obj_lda;
    opcode_information obj_ldb("68",3,true);
    op_tab["LDB"]=obj_ldb;
    opcode_information obj_ldch("50",3,true);
    op_tab["LDCH"]=obj_ldch;
    opcode_information obj_ldf("70",3,true);
    op_tab["LDF"]=obj_ldf;
    opcode_information obj_ldl("08",3,true);
    op_tab["LDL"]=obj_ldl;
    opcode_information obj_lds("6C",3,true);
    op_tab["LDS"]=obj_lds;
    opcode_information obj_ldt("74",3,true);
    op_tab["LDT"]=obj_ldt;
    opcode_information obj_ldx("04",3,true);
    op_tab["LDX"]=obj_ldx;
    opcode_information obj_lps("D0",3,true);
    op_tab["LPS"]=obj_lps;
    opcode_information obj_mul("20",3,true);
    op_tab["MUL"]=obj_mul;
    opcode_information obj_mulf("60",3,true);
    op_tab["MULF"]=obj_mulf;
    opcode_information obj_mulr("98",2,true);
    op_tab["MULR"]=obj_mulr;
    opcode_information obj_norm("C8",1,true);
    op_tab["NORM"]=obj_norm;
    opcode_information obj_or("44",3,true);
    op_tab["OR"]=obj_or;
    opcode_information obj_rd("D8",3,true);
    op_tab["RD"]=obj_rd;
    opcode_information obj_rmo("AC",2,true);
    op_tab["RMO"]=obj_rmo;
    opcode_information obj_rsub("4F",3,true);
    op_tab["RSUB"]=obj_rsub;
    opcode_information obj_shiftl("A4",2,true);
    op_tab["SHIFTL"]=obj_shiftl;
    opcode_information obj_shiftr("A8",2,true);
    op_tab["SHIFTR"]=obj_shiftr;
    opcode_information obj_sio("F0",1,true);
    op_tab["SIO"]=obj_sio;
    opcode_information obj_ssk("EC",3,true);
    op_tab["SSK"]=obj_ssk;
    opcode_information obj_sta("0C",3,true);
    op_tab["STA"]=obj_sta;
    opcode_information obj_stb("78",3,true);
    op_tab["STB"]=obj_stb;
    opcode_information obj_stch("54",3,true);
    op_tab["STCH"]=obj_stch;
    opcode_information obj_stf("80",3,true);
    op_tab["STF"]=obj_stf;
    opcode_information obj_sti("D4",3,true);
    op_tab["STI"]=obj_sti;
    opcode_information obj_stl("14",3,true);
    op_tab["STL"]=obj_stl;
    opcode_information obj_sts("7C",3,true);
    op_tab["STS"]=obj_sts;
    opcode_information obj_stsw("E8",3,true);
    op_tab["STSW"]=obj_stsw;
    opcode_information obj_stt("84",3,true);
    op_tab["STT"]=obj_stt;
    opcode_information obj_stx("10",3,true);
    op_tab["STX"]=obj_stx;
    opcode_information obj_sub("1C",3,true);
    op_tab["SUB"]=obj_sub;
    opcode_information obj_subf("5C",3,true);
    op_tab["SUBF"]=obj_subf;
    opcode_information obj_subr("94",2,true);
    op_tab["SUBR"]=obj_subr;
    opcode_information obj_svc("B0",2,true);
    op_tab["SVC"]=obj_svc;
    opcode_information obj_td("E0",3,true);
    op_tab["TD"]=obj_td;
    opcode_information obj_tio("F8",1,true);
    op_tab["TIO"]=obj_tio;
    opcode_information obj_tix("2C",3,true);
    op_tab["TIX"]=obj_tix;
    opcode_information obj_tixr("B8",2,true);
    op_tab["TIXR"]=obj_tixr;
    opcode_information obj_wd("DC",3,true);
    op_tab["WD"]=obj_wd;
}


void ld_regtab(){
    registers_information reg_a('0',true);
    reg_tab["A"]=reg_a;
    registers_information reg_x('1',true);
    reg_tab["X"]=reg_x;
    registers_information reg_l('2',true);
    reg_tab["L"]=reg_l;
    registers_information reg_b('3',true);
    reg_tab["B"]=reg_b;
    registers_information reg_s('4',true);
    reg_tab["S"]=reg_s;
    registers_information reg_t('5',true);
    reg_tab["T"]=reg_t;
    registers_information reg_f('6',true);
    reg_tab["F"]=reg_f;
    registers_information reg_pc('8',true);
    reg_tab["PC"]=reg_pc;
    registers_information reg_sw('9',true);
    reg_tab["SW"]=reg_sw;
}

void ld_blocks(){
  blocks_information obj(true,"DEFAULT","00000",0,"0");
  blocks["DEFAULT"]=obj;
}

void load_tables(){
  ld_regtab();ld_op_tab();ld_blocks();
}

