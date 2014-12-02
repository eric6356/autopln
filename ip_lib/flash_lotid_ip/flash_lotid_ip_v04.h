
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

//===program data==================
struct  row_bit
{
   unsigned    ra1  : 1;//low
   unsigned    ra2  : 1;
   unsigned    ra3  : 1;
   unsigned    ra4  : 1;
   unsigned    ra5  : 1;
   unsigned    ra6  : 1;
   unsigned    ra7  : 1;
   unsigned    ra8  : 1;//high
};

struct  row_asci
{
   unsigned    as_lo : 4;
   unsigned    as_hi : 4;
};

struct row_lot
{
   unsigned    lotid : 8;
};

union  ADDRESS
{
   struct  row_bit    bit;
   struct  row_asci   asci;
   struct  row_lot    lot;
   int     row_int;
} row,check;

//===check data========================
struct  addr_bit  {
   unsigned    ra1  : 1;//low
   unsigned    ra2  : 1;
   unsigned    ra3  : 1;
   unsigned    ra4  : 1;
   unsigned    ra5  : 1;
   unsigned    ra6  : 1;
   unsigned    ra7  : 1;
   unsigned    ra8  : 1;//high
};

struct   addr_fmt  {
         unsigned   addr_lo  :  4;
         unsigned   addr_hi  :  4;
};

struct   addr_int  {
         unsigned   digit    : 8;
};

union  dress  {
	 struct  addr_bit    bit;
         struct  addr_fmt    fmt;
         struct  addr_int    dgt;
} Addr,Addr_read;

//===structure define end===============

char which_flow[10];
char dec_file[50];
char TDO[20],TMS[20],TCK[20],TDI[20];
int get_id=-1;
char lotid_check_first[32];
char lotid_check_second[32];

int cycle_a,cycle_b;
char lotnumber[20],codename[20];
int partnumber[5],selfcode[6];

char testcode[50];
int ispcodename[3];
int flash_type;
FILE *file_pg,*file_rd,*file_w_codename,*file_r_codename;

void pg_pattern_head();
void pg_enter_mode();
void pg_addr_fixed();
void pg_addr_fixed_1();
void pg_ins42_format();
void pg_mid_data();
void pg_instr_end();
void pg_file_stop();

void rd_pattern_head();
void rd_enter_mode();
void rd_addr_fixed();
void rd_addr_fixed_1();
void rd_ins4a_format();
void rd_mid_data();
void rd_file_stop();

void isp_area_18_codename();
void isp_area_153_codename();

//----------------------------------------------------delete pattern and other associated files ,START
int flash_lotid_v04()
{
   char pgmxxxxx[150],rdxxxxx[150];
   char pgcodename[100],rdcodename[100];
   char pat[15]=".pat";


   system("del .\\..\\pat\\pgmxxxxx.pat");
   printf("del .\\..\\pat\\pgmxxxxx.pat\n");
   system("del .\\..\\pat\\rdxxxxx.pat");
   printf("del .\\..\\pat\\rdxxxxx.pat\n");
   system("del .\\..\\pat\\pgmxxxxx\\pgmxxxxx*.*");
   printf("del .\\..\\pat\\pgmxxxxx\\pgmxxxxx*.*\n");
   system("del .\\..\\pat\\rdxxxxx\\rdxxxxx*.*");
   printf("del .\\..\\pat\\rdxxxxx\\rdxxxxx*.*\n");
/*
   if(strcmp(which_flow,"code")==0)
   {
      system("del .\\..\\pat\\pgcodename.pat");
      printf("del .\\..\\pat\\pgcodename.pat\n");
      system("del .\\..\\pat\\rdcodename.pat");
      printf("del .\\..\\pat\\rdcodename.pat\n");
      system("del .\\..\\pat\\pgcodename\\pgcodename*.*");
      printf("del .\\..\\pat\\pgcodename\\pgcodename*.*\n");
      system("del .\\..\\pat\\rdcodename\\rdcodename*.*");
      printf("del .\\..\\pat\\rdcodename\\rdcodename*.*\n");
   }

   system("del .\\..\\pat\\*.pdz");
   printf("del .\\..\\pat\\*.pdz\n");
   system("del .\\..\\pat\\*.ppo");
   printf("del .\\..\\pat\\*.ppo\n");


   if(strcmp(which_flow,"code")==0)
   {
      if( (access("..\\PAT\\pgcodename.pat",0)==0)||(access("..\\PAT\\rdcodename.pat",0)==0) )
      {
         printf("now,isp pw blk codename pattern must have been deleted! but they still exist\n");
         return 2000;
      }
   }
    */ //ceci 14.6.16
   if( (access("..\\PAT\\pgmxxxxx.pat",0)==0)||(access("..\\PAT\\rdxxxxx.pat",0)==0) )
   {
      printf("now,pro blk pattern must have been deleted! but they still exist\n");
      return 2000;
   }

//------------------------------------------------------delete pattern and other associated files ,END


//--------------------------------------------------------check inputs format & preperation ,START
   if(strcmp(which_flow,"blank")==0)
   {
      if(strcmp(lotnumber,"")==0){
         printf("in blank flow, no lotnumber found !\n");
         return 0;
      }

      lotnumber[5]='\0';
      for(cycle_a=0;cycle_a<5;cycle_a++)
      {
         if( (lotnumber[cycle_a]>=97&&lotnumber[cycle_a]<=122)||(lotnumber[cycle_a]>=64&&lotnumber[cycle_a]<=90)||(lotnumber[cycle_a]>=48&&lotnumber[cycle_a]<=57) )
         {
            if(lotnumber[cycle_a]>=97&&lotnumber[cycle_a]<=122)
               lotnumber[cycle_a]=lotnumber[cycle_a]-32;
         }
         else
         {
            printf("in blank flow, lotnumber exists, but lotnumber inputs unformal !\n");
            return 1000;
         }
      }
   }
   else if(strcmp(which_flow,"code")==0)
   {
      if((strcmp(lotnumber,"")==0)||(strcmp(codename,"")==0)){
         printf("in code flow, no lotnumber/codename found  !\n");
         return 0;
      }

      lotnumber[5]='\0';
      codename[5]='\0';
      for(cycle_a=0;cycle_a<5;cycle_a++)
      {

         if( (lotnumber[cycle_a]>=97&&lotnumber[cycle_a]<=122)||(lotnumber[cycle_a]>=64&&lotnumber[cycle_a]<=90)||(lotnumber[cycle_a]>=48&&lotnumber[cycle_a]<=57) )
         {
            if(lotnumber[cycle_a]>=97&&lotnumber[cycle_a]<=122)
               lotnumber[cycle_a]=lotnumber[cycle_a]-32;
         }
         else
         {
            printf("in code flow, lotnumber exists, BUT lotnumber inputs unformal!\n");
            return 1000;
         }

         if( (codename[cycle_a]>=97&&codename[cycle_a]<=122)||(codename[cycle_a]>=64&&codename[cycle_a]<=90)||(codename[cycle_a]>=48&&codename[cycle_a]<=57) )
         {
            if(codename[cycle_a]>=97&&codename[cycle_a]<=122)
                 codename[cycle_a]=codename[cycle_a]-32;
         }
         else
         {
            printf("in code flow, code name exists, BUT code name inputs unformal!\n");
            return 1000;
         }
      }

   }
   else
   {
      printf("char var which_flow got error value !\n");
      return 2000;
   }

   if(strcmp(testcode,"")==0){
      printf("  no testcode found !\n");
      return 0;
   }

   testcode[32]='\0';
//   for(cycle_a=0;cycle_a<32;cycle_a++)
//   {
//      if( (testcode[cycle_a]>=97&&testcode[cycle_a]<=122)||(testcode[cycle_a]>=64&&testcode[cycle_a]<=90)||(testcode[cycle_a]>=48&&testcode[cycle_a]<=57)||(testcode[cycle_a]==95)||(testcode[cycle_a]==45) )
//      {
//         if(testcode[cycle_a]>=97&&testcode[cycle_a]<=122)
//            testcode[cycle_a]=testcode[cycle_a]-32;
//      }
//      else
//      {
//         printf("test code  exists, but test code inputs unformal !\n");
//         return 1000;
//      }
//   }

//--------------------------------------------------------check inputs format & preperation ,END

//--------------------------------------------------------CREAT AND PRINT pgmXXXX PATTERN, STRAT
//===========================================================
//===          the following for pg pattern               ===
//===========================================================


   strcpy(pgmxxxxx,".\\..\\pat\\pgmxxxxx");

   strcat(pgmxxxxx,pat);


   file_pg = fopen(pgmxxxxx,"w");

   pg_pattern_head();
   pg_enter_mode();

  for(cycle_a=0; cycle_a<=32; cycle_a++)
  {
     if(cycle_a==0)
     {
        row.lot.lotid=0xC0;
        Addr.dgt.digit=cycle_a;
        if(flash_type==0)pg_addr_fixed();   // .18  flash
        if(flash_type==1)pg_addr_fixed_1(); // .153 flash
        pg_ins42_format();
     }

     if(cycle_a==1)  row.lot.lotid=lotnumber[0];  //lotid
     if(cycle_a==2) {row.lot.lotid=lotnumber[1];check.lot.lotid=0xc0;}
     if(cycle_a==3) {row.lot.lotid=lotnumber[2];check.lot.lotid=lotnumber[0];}
     if(cycle_a==4) {row.lot.lotid=lotnumber[3];check.lot.lotid=lotnumber[1];}
     if(cycle_a==5) {row.lot.lotid=lotnumber[4];check.lot.lotid=lotnumber[2];}
     if(cycle_a==6) {row.lot.lotid=0x00;        check.lot.lotid=lotnumber[3];}
     if(cycle_a==7) {row.lot.lotid=0x00;        check.lot.lotid=lotnumber[4];}
     if(cycle_a==8) {row.lot.lotid=0x00;        check.lot.lotid=0x00;}
     if(cycle_a==9) {row.lot.lotid=partnumber[4];check.lot.lotid=0x00;}          //part number high
     if(cycle_a==10){row.lot.lotid=partnumber[3];check.lot.lotid=0x00;}          //             |
     if(cycle_a==11){row.lot.lotid=partnumber[2];check.lot.lotid=partnumber[4];} //             |
     if(cycle_a==12){row.lot.lotid=partnumber[1];check.lot.lotid=partnumber[3];} //            \|/
     if(cycle_a==13){row.lot.lotid=partnumber[0];check.lot.lotid=partnumber[2];} //            low
     if(cycle_a==14){row.lot.lotid=0x00;         check.lot.lotid=partnumber[1];} //reserved
     if(cycle_a==15){row.lot.lotid=0x00;         check.lot.lotid=partnumber[0];} //reserved

     if(strcmp(which_flow,"code")==0)
     {
        if(cycle_a==16) {row.lot.lotid=codename[0];check.lot.lotid=0x00;}  //codename,code flow
        if(cycle_a==17) {row.lot.lotid=codename[1];check.lot.lotid=0x00;}
        if(cycle_a==18) {row.lot.lotid=codename[2];check.lot.lotid=codename[0];}
        if(cycle_a==19) {row.lot.lotid=codename[3];check.lot.lotid=codename[1];}
        if(cycle_a==20) {row.lot.lotid=codename[4];check.lot.lotid=codename[2];}
     }
     else
     {
        if(cycle_a==16) {row.lot.lotid=0x00;check.lot.lotid=0x00;}  //codename,blank flow
        if(cycle_a==17) {row.lot.lotid=0x00;check.lot.lotid=0x00;}
        if(cycle_a==18) {row.lot.lotid=0x00;check.lot.lotid=0x00;}
        if(cycle_a==19) {row.lot.lotid=0x00;check.lot.lotid=0x00;}
        if(cycle_a==20) {row.lot.lotid=0x00;check.lot.lotid=0x00;}
     }

     if(cycle_a==21)              //selfcode & the last two bytes in check.lot.lotid
     {
        row.lot.lotid=selfcode[0]; row.lot.lotid=~row.lot.lotid;
        if(strcmp(which_flow,"code")==0)
        {
           check.lot.lotid=codename[3];
        }
        else check.lot.lotid=0x00;
     }
     if(cycle_a==22)
     {
     	row.lot.lotid=selfcode[0];
     	if(strcmp(which_flow,"code")==0)
       {
          check.lot.lotid=codename[4];
       }
       else check.lot.lotid=0x00;
     }
     if(cycle_a==23) {row.lot.lotid=selfcode[1]; row.lot.lotid=~row.lot.lotid;}
     if(cycle_a==24)  row.lot.lotid=selfcode[1];
     if(cycle_a==25) {row.lot.lotid=selfcode[2]; row.lot.lotid=~row.lot.lotid;}
     if(cycle_a==26)  row.lot.lotid=selfcode[2];
     if(cycle_a==27) {row.lot.lotid=selfcode[3]; row.lot.lotid=~row.lot.lotid;}
     if(cycle_a==28)  row.lot.lotid=selfcode[3];
     if(cycle_a==29) {row.lot.lotid=selfcode[4]; row.lot.lotid=~row.lot.lotid;}
     if(cycle_a==30)  row.lot.lotid=selfcode[4];
     if(cycle_a==31) {row.lot.lotid=selfcode[5]; row.lot.lotid=~row.lot.lotid;}
     if(cycle_a==32)  row.lot.lotid=selfcode[5];

     if( cycle_a!=0 )
     {
     	Addr.dgt.digit=cycle_a;
     	pg_mid_data();
     }

     if( cycle_a==32 )
     {
     	pg_instr_end();
     }
  }

  for(cycle_a=63; cycle_a<=69; cycle_a++)
  {
     if(cycle_a==63)
     {
        row.lot.lotid=0x00;
        Addr.dgt.digit=cycle_a;
        if(flash_type==0)pg_addr_fixed();   // .18  flash
        if(flash_type==1)pg_addr_fixed_1(); // .153 flash
        pg_ins42_format();
     }

     if(cycle_a==64) { row.lot.lotid=0x5A; }
     if(cycle_a==65) { row.lot.lotid=0xA5; }
     if(cycle_a==66) { row.lot.lotid=0x69; check.lot.lotid=0x5A; }
     if(cycle_a==67) { row.lot.lotid=0x96; check.lot.lotid=0xA5; }
     if(cycle_a==68) { row.lot.lotid=0x3C; check.lot.lotid=0x69; }
     if(cycle_a==69) { row.lot.lotid=0xC3; check.lot.lotid=0x96; }

     if( cycle_a!=63 )
     {
     	Addr.dgt.digit=cycle_a;
     	pg_mid_data();
     }

     if( cycle_a==69 )
     {
     	pg_instr_end();
     }
  }



  for(cycle_a=80; cycle_a<=113; cycle_a++)
  {
     if(cycle_a==80)
     {
        row.lot.lotid=testcode[0];
        Addr.dgt.digit=cycle_a;
        if(flash_type==0)pg_addr_fixed();   // .18  flash
        if(flash_type==1)pg_addr_fixed_1(); // .153 flash
        pg_ins42_format();
     }

     if(cycle_a==81) { row.lot.lotid=testcode[1]; }
     if(cycle_a==82) { row.lot.lotid=testcode[2];check.lot.lotid=testcode[0]; }
     if(cycle_a==83) { row.lot.lotid=testcode[3];check.lot.lotid=testcode[1]; }
     if(cycle_a==84) { row.lot.lotid=testcode[4];check.lot.lotid=testcode[2];}
     if(cycle_a==85) { row.lot.lotid=testcode[5];check.lot.lotid=testcode[3];}
     if(cycle_a==86) { row.lot.lotid=testcode[6];check.lot.lotid=testcode[4];}
     if(cycle_a==87) { row.lot.lotid=testcode[7];check.lot.lotid=testcode[5];}
     if(cycle_a==88) { row.lot.lotid=testcode[8];check.lot.lotid=testcode[6];}
     if(cycle_a==89) { row.lot.lotid=testcode[9];check.lot.lotid=testcode[7];}
     if(cycle_a==90) { row.lot.lotid=testcode[10];check.lot.lotid=testcode[8];}
     if(cycle_a==91) { row.lot.lotid=testcode[11];check.lot.lotid=testcode[9];}
     if(cycle_a==92) { row.lot.lotid=testcode[12];check.lot.lotid=testcode[10];}
     if(cycle_a==93) { row.lot.lotid=testcode[13];check.lot.lotid=testcode[11];}
     if(cycle_a==94) { row.lot.lotid=testcode[14];check.lot.lotid=testcode[12];}
     if(cycle_a==95) { row.lot.lotid=testcode[15];check.lot.lotid=testcode[13];}
     if(cycle_a==96) { row.lot.lotid=testcode[16];check.lot.lotid=testcode[14];}
     if(cycle_a==97) { row.lot.lotid=testcode[17];check.lot.lotid=testcode[15];}
     if(cycle_a==98) { row.lot.lotid=testcode[18];check.lot.lotid=testcode[16];}
     if(cycle_a==99) { row.lot.lotid=testcode[19];check.lot.lotid=testcode[17];}
     if(cycle_a==100) { row.lot.lotid=testcode[20];check.lot.lotid=testcode[18];}
     if(cycle_a==101) { row.lot.lotid=testcode[21];check.lot.lotid=testcode[19];}
     if(cycle_a==102) { row.lot.lotid=testcode[22];check.lot.lotid=testcode[20];}
     if(cycle_a==103) { row.lot.lotid=testcode[23];check.lot.lotid=testcode[21];}
     if(cycle_a==104) { row.lot.lotid=testcode[24];check.lot.lotid=testcode[22];}
     if(cycle_a==105) { row.lot.lotid=testcode[25];check.lot.lotid=testcode[23];}
     if(cycle_a==106) { row.lot.lotid=testcode[26];check.lot.lotid=testcode[24];}
     if(cycle_a==107) { row.lot.lotid=testcode[27];check.lot.lotid=testcode[25];}
     if(cycle_a==108) { row.lot.lotid=testcode[28];check.lot.lotid=testcode[26];}
     if(cycle_a==109) { row.lot.lotid=testcode[29];check.lot.lotid=testcode[27];}
     if(cycle_a==110) { row.lot.lotid=testcode[30];check.lot.lotid=testcode[28];}
     if(cycle_a==111) { row.lot.lotid=testcode[31];check.lot.lotid=testcode[29];}

     if(cycle_a==112) { row.lot.lotid=0x00;check.lot.lotid=testcode[30];}
     if(cycle_a==113) { row.lot.lotid=0x00;check.lot.lotid=testcode[31];}

     if( cycle_a!=80 )
     {
     	Addr.dgt.digit=cycle_a;
     	pg_mid_data();
     }

     if( cycle_a==113 )
     {
     	pg_instr_end();
     }
  }



  for(cycle_a=127; cycle_a<=135; cycle_a++)
  {
     if(cycle_a==127)
     {
        row.lot.lotid=0x00;
        Addr.dgt.digit=cycle_a;
        if(flash_type==0)pg_addr_fixed();   // .18  flash
        if(flash_type==1)pg_addr_fixed_1(); // .153 flash
        pg_ins42_format();
     }
     if(cycle_a==128) { row.lot.lotid=partnumber[4]; }
     if(cycle_a==129) { row.lot.lotid=partnumber[3]; }
     if(cycle_a==130) {row.lot.lotid=partnumber[2];check.lot.lotid=partnumber[4];}
     if(cycle_a==131) {row.lot.lotid=partnumber[1];check.lot.lotid=partnumber[3];}
     if(cycle_a==132) {row.lot.lotid=partnumber[0];check.lot.lotid=partnumber[2];}
     if(cycle_a==133) {row.lot.lotid=ispcodename[2];check.lot.lotid=partnumber[1];}
     if(cycle_a==134) {row.lot.lotid=ispcodename[1];check.lot.lotid=partnumber[0];}
     if(cycle_a==135) {row.lot.lotid=ispcodename[0];check.lot.lotid=ispcodename[2];}

     if( cycle_a!=127 )
     {
     	Addr.dgt.digit=cycle_a;
     	pg_mid_data();
     }

     if( cycle_a==135 )
     {
     	pg_instr_end();
     	pg_file_stop();
     }
  }

  fclose(file_pg);
//--------------------------------------------------------CREAT AND PRINT pgmXXXX PATTERN, END

//--------------------------------------------------------CREAT AND PRINT rdXXXX PATTERN, START

//===========================================================
//===          the following for rd pattern               ===
//===========================================================
  strcpy(rdxxxxx,".\\..\\pat\\rdxxxxx");
  strcat(rdxxxxx,pat);

  file_rd = fopen(rdxxxxx,"w");

  rd_pattern_head();
  rd_enter_mode();
  for(cycle_b=0; cycle_b<136; cycle_b++)
  {
     if(cycle_b==0)
     {
        row.lot.lotid=0xC0;
        if(flash_type==0)rd_addr_fixed();   // .18  flash
        if(flash_type==1)rd_addr_fixed_1(); // .153 flash
        rd_ins4a_format();
     }

     if(cycle_b==1) row.lot.lotid=lotnumber[0];  //lotid
     if(cycle_b==2) {row.lot.lotid=lotnumber[1];}
     if(cycle_b==3) {row.lot.lotid=lotnumber[2];}
     if(cycle_b==4) {row.lot.lotid=lotnumber[3];}
     if(cycle_b==5) {row.lot.lotid=lotnumber[4];}
     if(cycle_b==6) {row.lot.lotid=0x00;}
     if(cycle_b==7) {row.lot.lotid=0x00;}
     if(cycle_b==8) {row.lot.lotid=0x00;}
     if(cycle_b==9) {row.lot.lotid=partnumber[4];}          //part number high
     if(cycle_b==10){row.lot.lotid=partnumber[3];}          //             |
     if(cycle_b==11){row.lot.lotid=partnumber[2];} //             |
     if(cycle_b==12){row.lot.lotid=partnumber[1];} //            \|/
     if(cycle_b==13){row.lot.lotid=partnumber[0];} //            low
     if(cycle_b==14){row.lot.lotid=0x00;}  //reserved
     if(cycle_b==15){row.lot.lotid=0x00;}

     if(strcmp(which_flow,"code")==0)
     {
        if(cycle_b==16) {row.lot.lotid=codename[0];}  //codename,code flow
        if(cycle_b==17) {row.lot.lotid=codename[1];}
        if(cycle_b==18) {row.lot.lotid=codename[2];}
        if(cycle_b==19) {row.lot.lotid=codename[3];}
        if(cycle_b==20) {row.lot.lotid=codename[4];}
     }
     else
     {
        if(cycle_b==16) {row.lot.lotid=0x00;}  //codename,blank flow
        if(cycle_b==17) {row.lot.lotid=0x00;}
        if(cycle_b==18) {row.lot.lotid=0x00;}
        if(cycle_b==19) {row.lot.lotid=0x00;}
        if(cycle_b==20) {row.lot.lotid=0x00;}
     }

     if(cycle_b==64) { row.lot.lotid=0x5A; }
     if(cycle_b==65) { row.lot.lotid=0xA5; }
     if(cycle_b==66) { row.lot.lotid=0x69; }
     if(cycle_b==67) { row.lot.lotid=0x96; }
     if(cycle_b==68) { row.lot.lotid=0x3C; }
     if(cycle_b==69) { row.lot.lotid=0xC3; }




     if(cycle_b==80) {row.lot.lotid=testcode[0];}
     if(cycle_b==81) {row.lot.lotid=testcode[1];}
     if(cycle_b==82) {row.lot.lotid=testcode[2];}
     if(cycle_b==83) {row.lot.lotid=testcode[3];}
     if(cycle_b==84) {row.lot.lotid=testcode[4];}
     if(cycle_b==85) {row.lot.lotid=testcode[5];}
     if(cycle_b==86) {row.lot.lotid=testcode[6];}
     if(cycle_b==87) {row.lot.lotid=testcode[7];}
     if(cycle_b==88) {row.lot.lotid=testcode[8];}
     if(cycle_b==89) {row.lot.lotid=testcode[9];}

     if(cycle_b==90) {row.lot.lotid=testcode[10];}
     if(cycle_b==91) {row.lot.lotid=testcode[11];}
     if(cycle_b==92) {row.lot.lotid=testcode[12];}
     if(cycle_b==93) {row.lot.lotid=testcode[13];}
     if(cycle_b==94) {row.lot.lotid=testcode[14];}
     if(cycle_b==95) {row.lot.lotid=testcode[15];}
     if(cycle_b==96) {row.lot.lotid=testcode[16];}
     if(cycle_b==97) {row.lot.lotid=testcode[17];}
     if(cycle_b==98) {row.lot.lotid=testcode[18];}
     if(cycle_b==99) {row.lot.lotid=testcode[19];}

     if(cycle_b==100) {row.lot.lotid=testcode[20];}
     if(cycle_b==101) {row.lot.lotid=testcode[21];}
     if(cycle_b==102) {row.lot.lotid=testcode[22];}
     if(cycle_b==103) {row.lot.lotid=testcode[23];}
     if(cycle_b==104) {row.lot.lotid=testcode[24];}
     if(cycle_b==105) {row.lot.lotid=testcode[25];}
     if(cycle_b==106) {row.lot.lotid=testcode[26];}
     if(cycle_b==107) {row.lot.lotid=testcode[27];}
     if(cycle_b==108) {row.lot.lotid=testcode[28];}
     if(cycle_b==109) {row.lot.lotid=testcode[29];}
     if(cycle_b==110) {row.lot.lotid=testcode[30];}
     if(cycle_b==111) {row.lot.lotid=testcode[31];}



     if(cycle_b==128) { row.lot.lotid=partnumber[4];}
     if(cycle_b==129) { row.lot.lotid=partnumber[3];}
     if(cycle_b==130) {row.lot.lotid=partnumber[2];}
     if(cycle_b==131) {row.lot.lotid=partnumber[1];}
     if(cycle_b==132) {row.lot.lotid=partnumber[0];}
     if(cycle_b==133) {row.lot.lotid=ispcodename[2];}
     if(cycle_b==134) {row.lot.lotid=ispcodename[1];}
     if(cycle_b==135) {row.lot.lotid=ispcodename[0];}

     rd_mid_data();

     if( cycle_b==135 )
     {
        rd_file_stop();
     }
  }

  fclose(file_rd);


//--------------------------------------------------------CREAT AND PRINT rdXXXX PATTERN, END

//--------------------------------------------------------CREAT AND PRINT pgcodename & rdcodename PATTERN, START

 /*
   if(strcmp(which_flow,"code")==0)
  {
      strcpy(pgcodename,".\\..\\pat\\pgcodename");
      strcpy(rdcodename,".\\..\\pat\\rdcodename");
      strcat(pgcodename,pat);
      strcat(rdcodename,pat);
      file_w_codename = fopen(pgcodename,"w");
      file_r_codename = fopen(rdcodename,"w");
      for(cycle_a=0;cycle_a<5;cycle_a++)
      {
         if(cycle_a==0) row.lot.lotid=codename[0];
         if(cycle_a==1) row.lot.lotid=codename[1];
         if(cycle_a==2) {row.lot.lotid=codename[2];check.lot.lotid=codename[0];}
         if(cycle_a==3) {row.lot.lotid=codename[3];check.lot.lotid=codename[1];}
         if(cycle_a==4) {row.lot.lotid=codename[4];check.lot.lotid=codename[2];}
         if(flash_type==0)isp_area_18_codename();
         if(flash_type==1)isp_area_153_codename();
      }
      fclose(file_w_codename);
      fclose(file_r_codename);
  }
*///ceci 14.6.16
  return 1;
}
//--------------------------------------------------------CREAT AND PRINT pgcodename & rdcodename PATTERN, END


//------------------------------------------------------------------------------------------//
//                                                                                          //
//                                                                                          //
//                              SUBFUNCTION    BLOCK                                        //
//                                                                                          //
//                                                                                          //
//------------------------------------------------------------------------------------------//


void pg_pattern_head()
{
       fprintf(file_pg,"/**********************************/\n");
       fprintf(file_pg,"/*  FLASH PRODUCT BLOCK           */\n");
       fprintf(file_pg,"/*  lotnumber   No.: %c%c%c%c%c        */\n" ,lotnumber[0],lotnumber[1],lotnumber[2],lotnumber[3],lotnumber[4]);

       if(strcmp(which_flow,"code")==0)
       fprintf(file_pg,"/*  romcode No.: %c%c%c%c%c            */\n",codename[0],codename[1],codename[2],codename[3],codename[4]);
       fprintf(file_pg,"/**********************************/\n");
       fprintf(file_pg,"SET_DEC_FILE \"%s\"                 \n",dec_file);
       fprintf(file_pg,"HEADER\n");
       fprintf(file_pg,"%s,%s,%s,%s;                        \n",TCK,TDI,TDO,TMS);
       fprintf(file_pg,"SPM_PATTERN (pgmxxxxx) {            \n");
       fprintf(file_pg,"\n");
}

void rd_pattern_head()
{
       fprintf(file_rd,"/**********************************/\n");
       fprintf(file_rd,"/*  FLASH  PRODUCT BLOCK          */\n");
       fprintf(file_rd,"/*  lotnumber   No.: %c%c%c%c%c        */\n",lotnumber[0],lotnumber[1],lotnumber[2],lotnumber[3],lotnumber[4]);

       if(strcmp(which_flow,"code")==0)
       fprintf(file_rd,"/*  romcode No.: %c%c%c%c%c            */\n",codename[0],codename[1],codename[2],codename[3],codename[4]);
       fprintf(file_rd,"/**********************************/\n");
       fprintf(file_rd,"SET_DEC_FILE \"%s\"              \n",dec_file);
       fprintf(file_rd,"HEADER\n");
       fprintf(file_rd,"%s,%s,%s,%s;                     \n",TCK,TDI,TDO,TMS);
       fprintf(file_rd,"SPM_PATTERN (rdxxxxx) {             \n");
       fprintf(file_rd,"\n");
}

void pg_enter_mode()
{
       fprintf(file_pg,"         *1111 *TS3      ;/* -------------*/\n");
       fprintf(file_pg,"         *1111 *RPT 100  ;/* cmdrst =10uS */\n");
       fprintf(file_pg,"         *1111 *         ;/* ------------ */\n");
       fprintf(file_pg,"         *1110 *         ;/*              */\n");
       fprintf(file_pg,"         *1110 *         ;/*              */\n");
       fprintf(file_pg,"         *1110 *         ;/*              */\n");
       fprintf(file_pg,"         *0100 *         ;/* ------------ */\n");
       fprintf(file_pg,"         *1100 *         ;/*              */\n");
       fprintf(file_pg,"         *0000 *         ;/*              */\n");
       fprintf(file_pg,"         *1000 *         ;/*              */\n");
       fprintf(file_pg,"         *0000 *         ;/*              */\n");
       fprintf(file_pg,"         *1000 *         ;/*              */\n");
       fprintf(file_pg,"         *0100 *         ;/*              */\n");
       fprintf(file_pg,"         *1100 *         ;/*              */\n");
       fprintf(file_pg,"         *0000 *         ;/*              */\n");
       fprintf(file_pg,"         *1000 *         ;/*              */\n");
       fprintf(file_pg,"         *0000 *         ;/* mode judge   */\n");
       fprintf(file_pg,"         *0100 *         ;/*   = 0x96     */\n");
       fprintf(file_pg,"         *1100 *         ;/*              */\n");
       fprintf(file_pg,"         *0100 *         ;/*              */\n");
       fprintf(file_pg,"         *1100 *         ;/*              */\n");
       fprintf(file_pg,"         *0000 *         ;/*              */\n");
       fprintf(file_pg,"         *1000 *         ;/*              */\n");
       fprintf(file_pg,"         *0100 *         ;/*              */\n");
       fprintf(file_pg,"         *1100 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *RPT  640 ;/*  wait>=64uS  */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/* ------------ */\n");
       fprintf(file_pg,"         *00X0 *         ;/*  write mode  */\n");
       fprintf(file_pg,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*   ins= 49    */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*   ins= 46    */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *01X0 *         ;/*              */\n");
       fprintf(file_pg,"         *11X0 *         ;/*              */\n");
       fprintf(file_pg,"         *00X0 *         ;/*              */\n");
       fprintf(file_pg,"         *10X0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*   ins= f0    */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/*              */\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/*              */\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/*              */\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*   ins= FF    */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*              */\n");
       fprintf(file_pg,"         *01L0 *         ;/*              */\n");
       fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_pg,"         *00L0 *         ;/*              */\n");
       fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
}

void rd_enter_mode()
{
       fprintf(file_rd,"         *1111 *TS3      ;/* -------------*/\n");
       fprintf(file_rd,"         *1111 *RPT 100  ;/* cmdrst=10uS  */\n");
       fprintf(file_rd,"         *1111 *         ;/* ------------ */\n");
       fprintf(file_rd,"         *1110 *         ;/*              */\n");
       fprintf(file_rd,"         *1110 *         ;/*              */\n");
       fprintf(file_rd,"         *1110 *         ;/*              */\n");
       fprintf(file_rd,"         *0100 *         ;/* ------------ */\n");
       fprintf(file_rd,"         *1100 *         ;/*              */\n");
       fprintf(file_rd,"         *0000 *         ;/*              */\n");
       fprintf(file_rd,"         *1000 *         ;/*              */\n");
       fprintf(file_rd,"         *0000 *         ;/*              */\n");
       fprintf(file_rd,"         *1000 *         ;/*              */\n");
       fprintf(file_rd,"         *0100 *         ;/*              */\n");
       fprintf(file_rd,"         *1100 *         ;/*              */\n");
       fprintf(file_rd,"         *0000 *         ;/*              */\n");
       fprintf(file_rd,"         *1000 *         ;/*              */\n");
       fprintf(file_rd,"         *0000 *         ;/* mode judge   */\n");
       fprintf(file_rd,"         *0100 *         ;/*   = 0x96     */\n");
       fprintf(file_rd,"         *1100 *         ;/*              */\n");
       fprintf(file_rd,"         *0100 *         ;/*              */\n");
       fprintf(file_rd,"         *1100 *         ;/*              */\n");
       fprintf(file_rd,"         *0000 *         ;/*              */\n");
       fprintf(file_rd,"         *1000 *         ;/*              */\n");
       fprintf(file_rd,"         *0100 *         ;/*              */\n");
       fprintf(file_rd,"         *1100 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/* ------------ */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *RPT  640 ;/*  wait>=64uS  */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/* ------------ */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*   ins= 49    */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*   ins= 46    */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *01X0 *         ;/*              */\n");
       fprintf(file_rd,"         *11X0 *         ;/*              */\n");
       fprintf(file_rd,"         *00X0 *         ;/*              */\n");
       fprintf(file_rd,"         *10X0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*   ins= f0    */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/*              */\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/*              */\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/*              */\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*   ins= FF    */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*              */\n");
       fprintf(file_rd,"         *01L0 *         ;/*              */\n");
       fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_rd,"         *00L0 *         ;/*              */\n");
       fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
}
void pg_addr_fixed()
{
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= 40    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *0%dL0 *         ;/* -------------*/\n",Addr.bit.ra8);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra8);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra7);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra7);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra6);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra6);
        fprintf(file_pg,"         *0%dL0 *         ;/*   ins= %x%x    */\n",Addr.bit.ra5,Addr.fmt.addr_hi,Addr.fmt.addr_lo);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra5);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra4);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra4);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra3);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra3);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra2);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra2);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra1);
        fprintf(file_pg,"         *1%dL0 *         ;/*--------------*/\n",Addr.bit.ra1);
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= 41    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= 0a    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
}
void pg_addr_fixed_1()
{
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= 40    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *0%dL0 *         ;/* -------------*/\n",Addr.bit.ra8);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra8);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra7);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra7);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra6);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra6);
        fprintf(file_pg,"         *0%dL0 *         ;/*   ins= %x%x    */\n",Addr.bit.ra5,Addr.fmt.addr_hi,Addr.fmt.addr_lo);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra5);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra4);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra4);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra3);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra3);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra2);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",Addr.bit.ra2);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",Addr.bit.ra1);
        fprintf(file_pg,"         *1%dL0 *         ;/*--------------*/\n",Addr.bit.ra1);
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= 41    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*   ins= 12    */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
}
void rd_addr_fixed()
{
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 40    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 00    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 41    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 0a    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
}
void rd_addr_fixed_1()
{
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 40    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 00    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*   ins= 41    */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*   ins= 12    */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*   ins= ff    */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*              */\n");
        fprintf(file_rd,"         *01L0 *         ;/*              */\n");
        fprintf(file_rd,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_rd,"         *00L0 *         ;/*              */\n");
        fprintf(file_rd,"         *10L0 *         ;/* -------------*/\n");
}
void pg_ins42_format()
{
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= 42    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* addr= 12%x%x   */\n",Addr.fmt.addr_hi,Addr.fmt.addr_lo);
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *0%dL0 *         ;/* -------------*/\n",row.bit.ra8);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra8);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",row.bit.ra7);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra7);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",row.bit.ra6);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra6);
        fprintf(file_pg,"         *0%dL0 *         ;/*   ins= %x%x    */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra5);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",row.bit.ra4);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra4);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",row.bit.ra3);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra3);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",row.bit.ra2);
        fprintf(file_pg,"         *1%dL0 *         ;/*              */\n",row.bit.ra2);
        fprintf(file_pg,"         *0%dL0 *         ;/*              */\n",row.bit.ra1);
        fprintf(file_pg,"         *1%dL0 *         ;/*--------------*/\n",row.bit.ra1);
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *01L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins= a5    */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*    ins= 15   */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*    ins= 0a   */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*    ins= 09   */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*--------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *00L0 *         ;/* -------------*/\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*   ins=06     */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *01L0 *         ;/*              */\n");
        fprintf(file_pg,"         *11L0 *         ;/*              */\n");
        fprintf(file_pg,"         *00L0 *         ;/*              */\n");
        fprintf(file_pg,"         *10L0 *         ;/*--------------*/\n");
}

void pg_mid_data()
{
        if( (cycle_a==1)||(cycle_a==64)||(cycle_a==65)||(cycle_a==81)||(cycle_a==128)||(cycle_a==129) )
        {
           fprintf(file_pg,"         *00X0 *         ;/* addr= 12%x%x   */\n",Addr.fmt.addr_hi,Addr.fmt.addr_lo);
           fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *0%dX0 *         ;/* -------------*/\n",row.bit.ra8);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra8);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra7);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra7);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra6);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra6);
           fprintf(file_pg,"         *0%dX0 *         ;/*   ins= %x%x    */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra5);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra4);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra4);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra3);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra3);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra2);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra2);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra1);
           fprintf(file_pg,"         *1%dX0 *         ;/*--------------*/\n",row.bit.ra1);
           fprintf(file_pg,"         *00X0 *RPT 300  ;/* pgt=30uS     */\n");
           fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *00X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*     null     */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*--------------*/\n");
        }
        else if( ((cycle_a>=2)&&(cycle_a<=22))||((cycle_a>=66)&&(cycle_a<=69))||((cycle_a>=82)&&(cycle_a<=113))||((cycle_a>=130)&&(cycle_a<=135)) )
        {
           fprintf(file_pg,"         *00X0 *         ;/* addr= 12%x%x   */\n",Addr.fmt.addr_hi,Addr.fmt.addr_lo);
           fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *0%dH0 *         ;/* -------------*/\n",row.bit.ra8);
           fprintf(file_pg,"         *1%dH0 *         ;/*              */\n",row.bit.ra8);
           fprintf(file_pg,"         *0%d%c0 *         ;/*              */\n",row.bit.ra7,check.bit.ra1==1?'H':'L');
           fprintf(file_pg,"         *1%d%c0 *         ;/*              */\n",row.bit.ra7,check.bit.ra1==1?'H':'L');
           fprintf(file_pg,"         *0%d%c0 *         ;/*              */\n",row.bit.ra6,check.bit.ra2==1?'H':'L');
           fprintf(file_pg,"         *1%d%c0 *         ;/*              */\n",row.bit.ra6,check.bit.ra2==1?'H':'L');
           fprintf(file_pg,"         *0%d%c0 *         ;/*   ins= %x%x    */\n",row.bit.ra5,check.bit.ra3==1?'H':'L',row.asci.as_hi,row.asci.as_lo);
           fprintf(file_pg,"         *1%d%c0 *         ;/*   out= %x%x    */\n",row.bit.ra5,check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
           fprintf(file_pg,"         *0%d%c0 *         ;/*              */\n",row.bit.ra4,check.bit.ra4==1?'H':'L');
           fprintf(file_pg,"         *1%d%c0 *         ;/*              */\n",row.bit.ra4,check.bit.ra4==1?'H':'L');
           fprintf(file_pg,"         *0%d%c0 *         ;/*              */\n",row.bit.ra3,check.bit.ra5==1?'H':'L');
           fprintf(file_pg,"         *1%d%c0 *         ;/*              */\n",row.bit.ra3,check.bit.ra5==1?'H':'L');
           fprintf(file_pg,"         *0%d%c0 *         ;/*              */\n",row.bit.ra2,check.bit.ra6==1?'H':'L');
           fprintf(file_pg,"         *1%d%c0 *         ;/*              */\n",row.bit.ra2,check.bit.ra6==1?'H':'L');
           fprintf(file_pg,"         *0%d%c0 *         ;/*              */\n",row.bit.ra1,check.bit.ra7==1?'H':'L');
           fprintf(file_pg,"         *1%d%c0 *         ;/*--------------*/\n",row.bit.ra1,check.bit.ra7==1?'H':'L');
           fprintf(file_pg,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
           fprintf(file_pg,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');
           fprintf(file_pg,"         *00X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*     null     */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*--------------*/\n");
        }
        else if( (cycle_a>22)&&(cycle_a<33) )
        {
           fprintf(file_pg,"         *00X0 *         ;/* addr=12%x%x   */\n",Addr.fmt.addr_hi,Addr.fmt.addr_lo);
           fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *0%dH0 *         ;/*--------------*/\n",row.bit.ra8);
           fprintf(file_pg,"         *1%dH0 *         ;/*              */\n",row.bit.ra8);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra7);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra7);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra6);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra6);
           fprintf(file_pg,"         *0%dX0 *         ;/*  ins= %x%x     */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra5);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra4);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra4);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra3);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra3);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra2);
           fprintf(file_pg,"         *1%dX0 *         ;/*              */\n",row.bit.ra2);
           fprintf(file_pg,"         *0%dX0 *         ;/*              */\n",row.bit.ra1);
           fprintf(file_pg,"         *1%dX0 *         ;/*--------------*/\n",row.bit.ra1);
           fprintf(file_pg,"         *00X0 *RPT 300  ;/* pgt=30uS     */\n");
           fprintf(file_pg,"         *10X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *00X0 *         ;/* -------------*/\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*     null     */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*              */\n");
           fprintf(file_pg,"         *00X0 *         ;/*              */\n");
           fprintf(file_pg,"         *10X0 *         ;/*--------------*/\n");
        }
        else{;}
}

void pg_instr_end()
{
     if(cycle_a==32)
     {
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*  ins= aa     */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *RPT 300  ;/* pgt=30uS     */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*  ins= aa     */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*  ins= ff     */\n");
        fprintf(file_pg,"         *11X0 *         ;/*  out= XX     */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*  ins= ff     */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
     }
     else if(cycle_a==69 )
     {
        check.lot.lotid=0x3C;
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*  ins= aa     */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*  out= %x%x     */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*  ins= aa     */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");

        check.lot.lotid=0xC3;
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*  ins= ff     */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*  out= %x%x     */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/* ------------ */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*  ins= ff     */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
     }

     else if(cycle_a==113 )
     {
        check.lot.lotid=0x00;
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*  ins= aa     */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*  out= %x%x     */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*  ins= aa     */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");

        check.lot.lotid=0x00;
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*  ins= ff     */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*  out= %x%x     */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/* ------------ */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*  ins= ff     */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
     }



     else if(cycle_a==135)
     {
        check.lot.lotid=ispcodename[1];
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*  ins= aa     */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*  out= %x%x     */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*  ins= aa     */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");

        check.lot.lotid=ispcodename[0];
        fprintf(file_pg,"         *01H0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11H0 *         ;/*              */\n");
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*  ins= ff     */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*  out= %x%x     */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_pg,"         *01%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *11%c0 *         ;/* ------------ */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_pg,"         *00%c0 *         ;/*              */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *10%c0 *         ;/* ------------ */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_pg,"         *01X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*  ins= ff     */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/*              */\n");
        fprintf(file_pg,"         *01X0 *         ;/*              */\n");
        fprintf(file_pg,"         *11X0 *         ;/* ------------ */\n");
        fprintf(file_pg,"         *00X0 *         ;/*              */\n");
        fprintf(file_pg,"         *10X0 *         ;/* ------------ */\n");
     }
     else{;}
}

void pg_file_stop()
{
        fprintf(file_pg,"         *0000 *STOP     ;/* -----END---- */\n}\n");

}

void rd_ins4a_format()
{
	 fprintf(file_rd,"         *00L0 *         ;/* -------------*/\n");
         fprintf(file_rd,"         *10L0 *         ;/*              */\n");
         fprintf(file_rd,"         *01L0 *         ;/*              */\n");
         fprintf(file_rd,"         *11L0 *         ;/*              */\n");
         fprintf(file_rd,"         *00L0 *         ;/*              */\n");
         fprintf(file_rd,"         *10L0 *         ;/*              */\n");
         fprintf(file_rd,"         *00L0 *         ;/*   ins= 4a    */\n");
         fprintf(file_rd,"         *10L0 *         ;/*              */\n");
         fprintf(file_rd,"         *01L0 *         ;/*              */\n");
         fprintf(file_rd,"         *11L0 *         ;/*              */\n");
         fprintf(file_rd,"         *00L0 *         ;/*              */\n");
         fprintf(file_rd,"         *10L0 *         ;/*              */\n");
         fprintf(file_rd,"         *01L0 *         ;/*              */\n");
         fprintf(file_rd,"         *11L0 *         ;/*              */\n");
         fprintf(file_rd,"         *00L0 *         ;/*              */\n");
         fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
         fprintf(file_rd,"         *00L0 *         ;/* addr= 1200   */\n");
         fprintf(file_rd,"         *10L0 *         ;/*--------------*/\n");
}

void rd_mid_data()
{
      if( ((cycle_b>=0)&&(cycle_b<=20))||((cycle_b>=64)&&(cycle_b<=69))||((cycle_b>=80)&&(cycle_b<=111))||((cycle_b>=128)&&(cycle_b<=135)) )
      {
         Addr.dgt.digit=cycle_b+1;
         fprintf(file_rd,"         *01L0 *         ;/*--------------*/\n");
         fprintf(file_rd,"         *11L0 *         ;/*              */\n");
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra1==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/*              */\n",row.bit.ra1==1?'H':'L');
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra2==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/*              */\n",row.bit.ra2==1?'H':'L');
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra3==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/*              */\n",row.bit.ra3==1?'H':'L');
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra4==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/*  data= %x%x    */\n",row.bit.ra4==1?'H':'L',row.asci.as_hi,row.asci.as_lo);
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra5==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/*              */\n",row.bit.ra5==1?'H':'L');
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra6==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/*              */\n",row.bit.ra6==1?'H':'L');
         fprintf(file_rd,"         *01%c0 *         ;/*              */\n",row.bit.ra7==1?'H':'L');
         fprintf(file_rd,"         *11%c0 *         ;/* -------------*/\n",row.bit.ra7==1?'H':'L');
         fprintf(file_rd,"         *00%c0 *         ;/* addr= 12%x%x   */\n",row.bit.ra8==1?'H':'L',Addr.fmt.addr_hi,Addr.fmt.addr_lo);
         fprintf(file_rd,"         *10%c0 *         ;/* -------------*/\n",row.bit.ra8==1?'H':'L');
      }
      else
      {
      	 Addr.dgt.digit=cycle_b+1;
      	 fprintf(file_rd,"         *01L0 *         ;/*--------------*/\n");
         fprintf(file_rd,"         *11L0 *         ;/*              */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/*              */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/*              */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/*              */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/*  data=       */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/*              */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/*              */\n");
         fprintf(file_rd,"         *01X0 *         ;/*              */\n");
         fprintf(file_rd,"         *11X0 *         ;/* -------------*/\n");
         fprintf(file_rd,"         *00X0 *         ;/* addr= 12%x%x   */\n",Addr.fmt.addr_hi,Addr.fmt.addr_lo);
         fprintf(file_rd,"         *10X0 *         ;/* -------------*/\n");
      }

}

void rd_file_stop()
{
	 fprintf(file_rd,"         *0000 *STOP     ;\n}\n");
}

void isp_area_18_codename()
{
    if(cycle_a==0)
    {
       fprintf(file_w_codename,"/**********************************/\n");
       fprintf(file_w_codename,"/*  isp pw blk write codename pat */\n");
       fprintf(file_w_codename,"/*  romcode No.: %c%c%c%c%c            */\n"
                     ,codename[0],codename[1],codename[2],codename[3],codename[4]);
       fprintf(file_w_codename,"/**********************************/\n");
       fprintf(file_w_codename,"SET_DEC_FILE \"%s\"                 \n",dec_file);
       fprintf(file_w_codename,"HEADER\n");
       fprintf(file_w_codename,"%s,%s,%s,%s;                        \n",TCK,TDI,TDO,TMS);
       fprintf(file_w_codename,"SPM_PATTERN (pgcodename) {            \n");
       fprintf(file_w_codename,"\n");

       fprintf(file_r_codename,"/**********************************/\n");
       fprintf(file_r_codename,"/*  isp pw blk read codename pat  */\n");
       fprintf(file_r_codename,"/*  romcode No.: %c%c%c%c%c            */\n"
                     ,codename[0],codename[1],codename[2],codename[3],codename[4]);
       fprintf(file_r_codename,"/**********************************/\n");
       fprintf(file_r_codename,"SET_DEC_FILE \"%s\"              \n",dec_file);
       fprintf(file_r_codename,"HEADER\n");
       fprintf(file_r_codename,"%s,%s,%s,%s;                     \n",TCK,TDI,TDO,TMS);
       fprintf(file_r_codename,"SPM_PATTERN (rdcodename) {             \n");
       fprintf(file_r_codename,"\n");

       fprintf(file_w_codename,"         *1111 *TS3      ;/* -------------*/\n");
       fprintf(file_w_codename,"         *1111 *RPT 100  ;/* cmdrst =10uS */\n");
       fprintf(file_w_codename,"         *1111 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/* mode judge   */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*   = 0x96     */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *RPT  640 ;/*  wait>=64uS  */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*  write mode  */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*   ins= 49    */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*   ins= 46    */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= f0    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= FF    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       //cat 3.29
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 40    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 0d    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 41    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 09    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       //----------------------------------------------
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 42    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* addr=090d    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *0%dL0 *         ;/* -------------*/\n",row.bit.ra8);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra8);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*   ins= %x%x   */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra5);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra1);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*--------------*/\n",row.bit.ra1);
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
//----------------------------------------------------------------------------------------------------
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= a5    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*    ins= 15   */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*    ins= 0a   */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*    ins= 09   */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins=06     */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");

       //read codename for once
       fprintf(file_r_codename,"         *1111 *TS3      ;/* -------------*/\n");
       fprintf(file_r_codename,"         *1111 *RPT 100  ;/* cmdrst =10uS */\n");
       fprintf(file_r_codename,"         *1111 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/* mode judge   */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*   = 0x96     */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *RPT  640 ;/*  wait>=64uS  */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*  write mode  */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*   ins= 49    */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*   ins= 46    */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= f0    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= FF    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       //cat 3.29
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 40    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 0d    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 41    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 09    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 4a    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");

       int cycle_isb_codename;
       for(cycle_isb_codename=0;cycle_isb_codename<5;cycle_isb_codename++)
       {
          row.lot.lotid=codename[cycle_isb_codename];
          fprintf(file_r_codename,"         *01L0 *         ;/*--------------*/\n");
          fprintf(file_r_codename,"         *11L0 *         ;/* addr= 090%c   */\n",98+cycle_isb_codename);
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra1==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra1==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra2==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra2==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra3==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra3==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra4==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*  data= %x%x  */\n",row.bit.ra4==1?'H':'L',row.asci.as_hi,row.asci.as_lo);
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra5==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra5==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra6==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra6==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra7==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/* -------------*/\n",row.bit.ra7==1?'H':'L');
          fprintf(file_r_codename,"         *00%c0 *         ;/*              */\n",row.bit.ra8==1?'H':'L');
          fprintf(file_r_codename,"         *10%c0 *         ;/* -------------*/\n",row.bit.ra8==1?'H':'L');
          if(cycle_isb_codename==4)
             fprintf(file_r_codename,"         *0000 *STOP  ;\n}\n");
       }
    }

    if(cycle_a==1)
    {
       fprintf(file_w_codename,"         *00L0 *         ;/* addr=090%c    */\n",98+cycle_a);
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *0%dL0 *         ;/* -------------*/\n",row.bit.ra8);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra8);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*   ins= %x%x   */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra5);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra1);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*--------------*/\n",row.bit.ra1);
       fprintf(file_w_codename,"         *00L0 *RPT 300  ;/* pgt=30uS     */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");

       fprintf(file_w_codename,"         *00L0 *         ; /* ------------ */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*  ins= 00     */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /* ------------ */\n");
    }
    if(cycle_a>=2&&cycle_a<=4)
    {
       fprintf(file_w_codename,"         *00L0 *         ;/* addr=090%c    */\n",98+cycle_a);
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *0%dH0 *         ;/* -------------*/\n",row.bit.ra8);
       fprintf(file_w_codename,"         *1%dH0 *         ;/*              */\n",row.bit.ra8);
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra7,check.bit.ra1==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra7,check.bit.ra1==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra6,check.bit.ra2==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra6,check.bit.ra2==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*   ins= %x%x   */\n",row.bit.ra5,check.bit.ra3==1?'H':'L',row.asci.as_hi,row.asci.as_lo);
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*   check= %x%x */\n",row.bit.ra5,check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra4,check.bit.ra4==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra4,check.bit.ra4==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra3,check.bit.ra5==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra3,check.bit.ra5==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra2,check.bit.ra6==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra2,check.bit.ra6==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra1,check.bit.ra7==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*--------------*/\n",row.bit.ra1,check.bit.ra7==1?'H':'L');
       fprintf(file_w_codename,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
       fprintf(file_w_codename,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');

       fprintf(file_w_codename,"         *00L0 *         ; /* ------------ */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*  ins= 00     */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /* ------------ */\n");
    }
    if(cycle_a==4)
    {
        check.lot.lotid=codename[3];
     	  fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *01H0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11H0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*   ins= aa    */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*   check= %x%x */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*--------------*/\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*   ins= aa    */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*--------------*/\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
        check.lot.lotid=codename[4];
        fprintf(file_w_codename,"         *01H0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11H0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*   ins= ff    */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*   check= %x%x */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*--------------*/\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *0000 *STOP  ;\n}\n");
     }


}

void isp_area_153_codename()
{
    if(cycle_a==0)
    {
       fprintf(file_w_codename,"/**********************************/\n");
       fprintf(file_w_codename,"/*  isp pw blk write codename pat */\n");
       fprintf(file_w_codename,"/*  romcode No.: %c%c%c%c%c            */\n"
                     ,codename[0],codename[1],codename[2],codename[3],codename[4]);
       fprintf(file_w_codename,"/**********************************/\n");
       fprintf(file_w_codename,"SET_DEC_FILE \"%s\"                 \n",dec_file);
       fprintf(file_w_codename,"HEADER\n");
       fprintf(file_w_codename,"%s,%s,%s,%s;                        \n",TCK,TDI,TDO,TMS);
       fprintf(file_w_codename,"SPM_PATTERN (pgcodename) {            \n");
       fprintf(file_w_codename,"\n");

       fprintf(file_r_codename,"/**********************************/\n");
       fprintf(file_r_codename,"/*  isp pw blk read codename pat  */\n");
       fprintf(file_r_codename,"/*  romcode No.: %c%c%c%c%c            */\n"
                     ,codename[0],codename[1],codename[2],codename[3],codename[4]);
       fprintf(file_r_codename,"/**********************************/\n");
       fprintf(file_r_codename,"SET_DEC_FILE \"%s\"              \n",dec_file);
       fprintf(file_r_codename,"HEADER\n");
       fprintf(file_r_codename,"%s,%s,%s,%s;                     \n",TCK,TDI,TDO,TMS);
       fprintf(file_r_codename,"SPM_PATTERN (rdcodename) {             \n");
       fprintf(file_r_codename,"\n");

       fprintf(file_w_codename,"         *1111 *TS3      ;/* -------------*/\n");
       fprintf(file_w_codename,"         *1111 *RPT 100  ;/* cmdrst =10uS */\n");
       fprintf(file_w_codename,"         *1111 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/* mode judge   */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*   = 0x96     */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_w_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *RPT  640 ;/*  wait>=64uS  */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/* ------------ */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*  write mode  */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*   ins= 49    */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*   ins= 46    */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10X0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= f0    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= FF    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       //cat 3.29
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 40    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 0d    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 41    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= 11    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       //----------------------------------------------
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= 42    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* addr=110d    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *0%dL0 *         ;/* -------------*/\n",row.bit.ra8);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra8);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*   ins= %x%x   */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra5);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra1);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*--------------*/\n",row.bit.ra1);
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
//----------------------------------------------------------------------------------------------------
       fprintf(file_w_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins= a5    */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*    ins= 15   */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*    ins= 0a   */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*    ins= 09   */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*   ins=06     */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/*--------------*/\n");

       //read codename for once
       fprintf(file_r_codename,"         *1111 *TS3      ;/* -------------*/\n");
       fprintf(file_r_codename,"         *1111 *RPT 100  ;/* cmdrst =10uS */\n");
       fprintf(file_r_codename,"         *1111 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1110 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/* mode judge   */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*   = 0x96     */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1000 *         ;/*              */\n");
       fprintf(file_r_codename,"         *0100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *1100 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *RPT  640 ;/*  wait>=64uS  */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/* ------------ */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*  write mode  */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*   ins= 49    */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00X0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*   ins= 46    */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00X0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10X0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= f0    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= FF    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       //cat 3.29
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 40    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 0d    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 41    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= 11    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *01L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*   ins= ff    */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/* -------------*/\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*   ins= 4a    */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *01L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *11L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");
       fprintf(file_r_codename,"         *00L0 *         ;/*              */\n");
       fprintf(file_r_codename,"         *10L0 *         ;/*--------------*/\n");

       int cycle_isb_codename;
       for(cycle_isb_codename=0;cycle_isb_codename<5;cycle_isb_codename++)
       {
          row.lot.lotid=codename[cycle_isb_codename];
          fprintf(file_r_codename,"         *01L0 *         ;/*--------------*/\n");
          fprintf(file_r_codename,"         *11L0 *         ;/* addr= 110%c   */\n",98+cycle_isb_codename);
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra1==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra1==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra2==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra2==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra3==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra3==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra4==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*  data= %x%x  */\n",row.bit.ra4==1?'H':'L',row.asci.as_hi,row.asci.as_lo);
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra5==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra5==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra6==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/*              */\n",row.bit.ra6==1?'H':'L');
          fprintf(file_r_codename,"         *01%c0 *         ;/*              */\n",row.bit.ra7==1?'H':'L');
          fprintf(file_r_codename,"         *11%c0 *         ;/* -------------*/\n",row.bit.ra7==1?'H':'L');
          fprintf(file_r_codename,"         *00%c0 *         ;/*              */\n",row.bit.ra8==1?'H':'L');
          fprintf(file_r_codename,"         *10%c0 *         ;/* -------------*/\n",row.bit.ra8==1?'H':'L');
          if(cycle_isb_codename==4)
             fprintf(file_r_codename,"         *0000 *STOP  ;\n}\n");
       }
    }

    if(cycle_a==1)
    {
       fprintf(file_w_codename,"         *00L0 *         ;/* addr=110%c    */\n",98+cycle_a);
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *0%dL0 *         ;/* -------------*/\n",row.bit.ra8);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra8);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra7);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra6);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*   ins= %x%x   */\n",row.bit.ra5,row.asci.as_hi,row.asci.as_lo);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra5);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra4);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra3);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*              */\n",row.bit.ra2);
       fprintf(file_w_codename,"         *0%dL0 *         ;/*              */\n",row.bit.ra1);
       fprintf(file_w_codename,"         *1%dL0 *         ;/*--------------*/\n",row.bit.ra1);
       fprintf(file_w_codename,"         *00L0 *RPT 300  ;/* pgt=30uS     */\n");
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");

       fprintf(file_w_codename,"         *00L0 *         ; /* ------------ */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*  ins= 00     */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /* ------------ */\n");
    }
    if(cycle_a>=2&&cycle_a<=4)
    {
       fprintf(file_w_codename,"         *00L0 *         ;/* addr=110%c    */\n",98+cycle_a);
       fprintf(file_w_codename,"         *10L0 *         ;/* -------------*/\n");
       fprintf(file_w_codename,"         *0%dH0 *         ;/* -------------*/\n",row.bit.ra8);
       fprintf(file_w_codename,"         *1%dH0 *         ;/*              */\n",row.bit.ra8);
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra7,check.bit.ra1==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra7,check.bit.ra1==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra6,check.bit.ra2==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra6,check.bit.ra2==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*   ins= %x%x   */\n",row.bit.ra5,check.bit.ra3==1?'H':'L',row.asci.as_hi,row.asci.as_lo);
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*   check= %x%x */\n",row.bit.ra5,check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra4,check.bit.ra4==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra4,check.bit.ra4==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra3,check.bit.ra5==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra3,check.bit.ra5==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra2,check.bit.ra6==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*              */\n",row.bit.ra2,check.bit.ra6==1?'H':'L');
       fprintf(file_w_codename,"         *0%d%c0 *         ;/*              */\n",row.bit.ra1,check.bit.ra7==1?'H':'L');
       fprintf(file_w_codename,"         *1%d%c0 *         ;/*--------------*/\n",row.bit.ra1,check.bit.ra7==1?'H':'L');
       fprintf(file_w_codename,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
       fprintf(file_w_codename,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');

       fprintf(file_w_codename,"         *00L0 *         ; /* ------------ */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*  ins= 00     */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *00L0 *         ; /*              */\n");
       fprintf(file_w_codename,"         *10L0 *         ; /* ------------ */\n");
    }
    if(cycle_a==4)
    {
        check.lot.lotid=codename[3];
      	fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *01H0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11H0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*   ins= aa    */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*   check= %x%x */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/*--------------*/\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *RPT 300  ;/* pgt=30uS     */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*   ins= aa    */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/*--------------*/\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
        check.lot.lotid=codename[4];
        fprintf(file_w_codename,"         *01H0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11H0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra1==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra2==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*   ins= ff    */\n",check.bit.ra3==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*   check= %x%x */\n",check.bit.ra3==1?'H':'L',check.asci.as_hi,check.asci.as_lo);
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra4==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra5==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*              */\n",check.bit.ra6==1?'H':'L');
        fprintf(file_w_codename,"         *01%c0 *         ;/*              */\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *11%c0 *         ;/*--------------*/\n",check.bit.ra7==1?'H':'L');
        fprintf(file_w_codename,"         *00%c0 *         ;/*              */\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *10%c0 *         ;/* -------------*/\n",check.bit.ra8==1?'H':'L');
        fprintf(file_w_codename,"         *01X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*   ins= ff    */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *01X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *11X0 *         ;/*--------------*/\n");
        fprintf(file_w_codename,"         *00X0 *         ;/*              */\n");
        fprintf(file_w_codename,"         *10X0 *         ;/* -------------*/\n");
        fprintf(file_w_codename,"         *0000 *STOP  ;\n}\n");
     }


}