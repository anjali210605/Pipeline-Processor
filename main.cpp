#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<map>
#include<stack>
#include<queue>
#include<algorithm>
#include<iterator>
#include<cassert>
#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <set>
using namespace std;


int str_intg(string s){
    int t1,t2;
   if(int(s[0])>47 && int(s[0])<58){
    t1=int(s[0])-48;
   }else if(int(s[0])>=97 && int(s[0])<=102){
    t1=int(s[0])-87;
   }
    if(int(s[1])>47 && int(s[1])<58){
    t2=int(s[1])-48;
   }else if(int(s[1])>=97 && int(s[1])<=102){
    t2=int(s[1])-87;
   }
   return 16*t1+t2;
}

string intg_str(int t){
   int s[2];
   s[0]=t/16;
   s[1]=t%16;
  if(s[0]>=10 && s[0]<=15){
    s[0]+=87;
  }else if(s[0]>=0 && s[0]<=9){
    s[0]+=48;
  }
  if(s[1]>=10 && s[1]<=15){
    s[1]+=87;
  }else if(s[1]>=0 && s[1]<=9){
    s[1]+=48;
  }
  char u[2];
 u[0]=char(s[0]);
 u[1]=char(s[1]);
 u[2]='\0';
return u;
}


int main(int argc, char *argv[]){
     string Dcache = "";
     string Icache = "";
      string rf = "";
  string DcacheOut = "DCache.txt";
   string Output = "output.txt";
  struct option longopts[] = {
		{"dcache", required_argument, NULL, 'a'}, 
		{"icache", required_argument, NULL, 'b'},
		{"rf", required_argument, NULL, 'c'},
    {"dcacheout", required_argument, NULL, 'd'},
    {"out", required_argument, NULL, 'e'},
		{0, 0, 0, 0}
    };
while(1){
    	const int opt = getopt_long(argc, argv, "", longopts, 0);

    	// unexpected error or done parsing
    	if (opt == -1){
    		break;
    	}

    	switch (opt){
    	case 'a':
    		// the value of the argument is stored in 'optarg' variable
    		Dcache = optarg;
    		break;
      case 'b':
    		// the value of the argument is stored in 'optarg' variable
    		Icache = optarg;
    		break;
    	case 'c':
    		rf = optarg;
    		break;
    	case 'd':
    		DcacheOut = optarg;
    		break;
        case 'e':
    		Output = optarg;
    		break;
    	case '?':
    		abort ();
		default:
			abort ();
    	}
    }
    ifstream ifile;
        string temp;
    ifile.open(rf);
    vector<int > regi(16);
    ifile>>temp;
    int i=0;
    while(i!=16){
        regi[i]=str_intg(temp);
        i++;
        ifile>>temp;
    }
    ifile.close();
    ifile.open(Dcache);
    vector<int > Data(256);
    ifile>>temp;
    i=0;
    while(i!=256){
        Data[i]=str_intg(temp);
        i++;
        ifile>>temp;
    }
    ifile.close();
    ifile.open(Icache);
    vector<int > inst(1000);
     ifile>>temp;
    i=0;
    while(i!=256){
        inst[i]=str_intg(temp);
        i++;
        ifile>>temp;
    }
    ifile.close();
    int stop;
    int d[5]={1,0,0,0,0};
    //write back;
   int opcode_wb,r1_wb,r2_wb,w_wb,alu_wb;
    //MEM
    int opcode_mem,r1_mem,r2_mem,w_mem,alu_mem;
    //EX
    int opcode_ex,r1_ex,r2_ex,w_ex,alu_ex,L1_ex;
    int imm_ex[2];
    //ID
    int opcode_id,r1_id,r2_id,w_id,alu_id,L1_id;
    int imm_id[2];
    //IF
    int PC=0;
    int total_inst=0;
    int jmp_inst=0;
    int bad_inst=0;
    int track=0;
    int datastalls=0;
    int ainst=0,linst=0,sinst=0,minst=0,liinst=0,cinst=0,hinst=0;
    queue<int > nop;
    while(1){
        if(d[4]==1){
          if(opcode_wb!=12 && opcode_wb!=13 && opcode_wb!=14 && opcode_wb!=15){
            regi[w_wb]=alu_wb;

          }
          if(opcode_wb==15){
            break;
          }
        }
         //MEM
        if(d[3]==1){
            if(opcode_mem==11){
               alu_mem=Data[alu_mem];
            }else if(opcode_mem==12){
                Data[alu_mem]=regi[r1_mem];
            }
            opcode_wb=opcode_mem;
            r1_wb=r1_mem;
            r2_wb=r2_mem;
            w_wb=w_mem;
            alu_wb=alu_mem;
        }
        //EX
        if(d[2]==1){
            if(opcode_ex==0){
              alu_ex=(regi[r1_ex]+regi[r2_ex])%256;
            } else if(opcode_ex==1){
              alu_ex=(regi[r1_ex]-regi[r2_ex])%256;
            } else if(opcode_ex==2){
              alu_ex=(regi[r1_ex]*regi[r2_ex])%256;
            } else if(opcode_ex==3){
              alu_ex=regi[r1_ex]+1;
            } else if(opcode_ex==4){
              alu_ex=regi[r1_ex] & regi[r2_ex];
            } else if(opcode_ex==5){
              alu_ex=regi[r1_ex]|regi[r2_ex];
            } else if(opcode_ex==6){
              alu_ex=regi[r1_ex]^regi[r2_ex];
            } else if(opcode_ex==7){
              alu_ex=~regi[r1_ex];
            } else if(opcode_ex==8){
              alu_ex=(regi[r1_ex]<<imm_ex[0])%256;
            }  else if(opcode_ex==9){
              alu_ex=(regi[r1_ex]>>imm_ex[0])%256;
            }  else if(opcode_ex==10){
              alu_ex=imm_ex[0]*16+imm_ex[1];
            }  else if(opcode_ex==11){
              alu_ex=regi[r1_ex]+imm_ex[0];
            }  else if(opcode_ex==12){
              alu_ex=regi[r2_ex]+imm_ex[0];
            }  else if(opcode_ex==13){
             if(r1_ex<=7){
                PC+=(16*(r1_ex)+L1_ex)*2;
              } else if(r1_ex>=8 && r1_ex<=15){
                PC-=(256-(16*(r1_ex)+L1_ex))*2;
              }
            } else if(opcode_ex==14){
              if(regi[r1_ex]==r2_ex){
                PC+=(L1_ex)*2;
              }
            }
            opcode_mem=opcode_ex;
            r1_mem=r1_ex;
            r2_mem=r2_ex;
            w_mem=w_ex;
            alu_mem=alu_ex;
        } 
         //ID
         if(d[1]==1){
            opcode_ex=opcode_id;
            r1_ex=r1_id;
            r2_ex=r2_id;
            w_ex=w_id;
            L1_ex=L1_id;
            imm_ex[0]=imm_id[0];
            imm_ex[1]=imm_id[1];
         }


         //IF

        if(nop.empty()){
          if(d[0]==1){
            opcode_id=inst[PC]/16;
            if(opcode_id>=0 && opcode_id<=11){
              w_id=inst[PC]%16;
              r1_id=inst[PC+1]/16;
              r2_id=inst[PC+1]%16;
            } 
            if(opcode_id==3){
              r1_id=w_id;
            }
            if(opcode_id==8 || opcode_id==9){
              imm_id[0]=inst[PC+1]/16;
            }
            if(opcode_id==10){
              r1_id=-1;
              r2_id=-1;
              w_id=inst[PC]%16;
              imm_id[0]=inst[PC+1]/16;
              imm_id[1]=inst[PC+1]%16;
            }
            if(opcode_id==11){
              imm_id[0]=inst[PC+1]%16;
              r2_id=-1;
            }
            if(opcode_id==12){
              w_id=-1;
              r1_id=inst[PC]%16;
              r2_id=inst[PC+1]/16;
              imm_id[0]=inst[PC+1]%16;
            }
              if(opcode_id==13){
                r1_id=inst[PC]%16;
                L1_id=inst[PC+1]/16;
            }
            if(opcode_id==14){
                r1_id=inst[PC]%16;
                r2_id=inst[PC+1]/16;
                L1_id=inst[PC+1]%16;
            }
             if(track>=3)
            if(((r1_id==w_mem && r1_id!=-1 && w_mem!=-1) || (r2_id==w_mem&& r1_id!=-1 && w_mem!=-1)) && opcode_mem!=18 && opcode_id!=18){
                // cout<<"1:opcode:"<<opcode_id<<"datastalls:"<<datastalls<<"total inst"<<total_inst<<endl;
                opcode_id=18;
                datastalls++;
                PC-=2;
            }
            if(track>=2)
            if(((r1_id==w_ex && r1_id!=-1 && w_ex!=-1) || (r2_id==w_ex && r1_id!=-1 && w_ex!=-1)) && opcode_ex!=18 && opcode_id!=18 ){
                // cout<<"2:opcode:"<<opcode_id<<"datastalls:"<<datastalls<<"total inst"<<total_inst<<endl;             
                opcode_id=18;
                nop.push(1);
                datastalls+=2;
                PC-=2;
            }
            if(opcode_id==13){
                r1_id=inst[PC]%16;
                L1_id=inst[PC+1]/16;
                nop.push(1);
                nop.push(1);
            }
            if(opcode_id==14){
                r1_id=inst[PC]%16;
                r2_id=inst[PC+1]/16;
                L1_id=inst[PC+1]%16;
                nop.push(1);
                nop.push(1);
            }
            if(opcode_id==15){
              nop.push(1);
              nop.push(1);
              nop.push(1);
              nop.push(1);
            }
            if(opcode_id==18){
               r1_id=-1;
                r2_id=-1;
                w_id=-1;
            }
            PC+=2;
        }
        }else{
          opcode_id=18;
          nop.pop();
        }
        //counting total instructions,branch and jump for counting control stalls
            if(opcode_id>=0 && opcode_id<=15){
                total_inst++;
                if(opcode_id==14){
                    bad_inst++;
                }else if(opcode_id==13){
                    jmp_inst++;
                }
            }
        //counting all instructions
          if(opcode_id>=0 && opcode_id<=3){
            ainst++;
          }else if(opcode_id>=0 && opcode_id<=7){
            linst++;
          }else if(opcode_id==8 || opcode_id==9){
            sinst++;
          }else if(opcode_id>=10 && opcode_id<=12){
             if(opcode_id==10){
            liinst++;
          } else minst++;
          }else if(opcode_id==13 || opcode_id==14){
            cinst++;
          }else if(opcode_id==15){
            hinst++;
          }
          //initial process
          if(track==0){
            d[1]=1;
          }else if(track==1){
            d[2]=1;
          }else if(track==2){
            d[3]=1;
          }else if(track==3){
            d[4]=1;
          }
          track++;
    }
    ofstream ofile;
    ofile.open(DcacheOut);
    for(int i=0;i<256;i++){
      if(Data[i]<0){
        Data[i]=Data[i]+256;
        Data[i]=Data[i]%256;
      }
      ofile<<intg_str(Data[i])<<endl;
    }
    ofile.close();
    ofile.open(Output);

    ofile<<"Total number of instructions executed        : "<<total_inst<<endl;
    ofile<<"Number of instructions in each class     "<<endl;
    ofile<<"Arithmetic instructions                      : "<<ainst<<endl;
    ofile<<"Logical instructions                         : "<<linst<<endl;
    ofile<<"Shift instructions                           : "<<sinst<<endl;
    ofile<<"Memory instructions                          : "<<minst<<endl;
    ofile<<"Load immediate instructions                  : "<<liinst<<endl;
    ofile<<"Control instructions                         : "<<cinst<<endl;
    ofile<<"Halt instructions                            : "<<hinst<<endl;
    ofile<<"Cycles Per Instruction                       : "<<(double)(total_inst+datastalls+(bad_inst+jmp_inst)*2+4)/(double)total_inst<<endl;
    ofile<<"Total number of stalls                       : "<<datastalls+(bad_inst+jmp_inst)*2<<endl;
    ofile<<"Data stalls (RAW)                            : "<<datastalls<<endl;
    ofile<<"Control stalls                               : "<<(bad_inst+jmp_inst)*2<<endl;
    ofile.close();
}