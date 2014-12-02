# coding:utf-8
from __future__ import with_statement
import sys
import os
import re

def main():
    lvl_keywords = ['high', 'typical', 'low', 'schmit1', 'schmit2', 'schmit3']
    # 获取从config.py传递的参数
    project_path = sys.argv[1]
    sys.path.append(project_path)
    import config as cfg
    item_list = map(lambda x: x.strip(), cfg.item_list.strip().split('\n'))
    high_level = cfg.high_level
    typical_level = cfg.typical_level
    low_level = cfg.low_level
    flash_lotid = cfg.flash_lotid
    schmit1 = cfg.schmit1
    schmit2 = cfg.schmit2
    schmit3 = cfg.schmit3
    vout1 = cfg.vout1
    vout2 = cfg.vout2
    vout3 = cfg.vout3
    vout4 = cfg.vout4

    # 若打开工程目录下有pattern.head，则从中获取dec文件名
    dec_filename = ''
    if os.path.exists(project_path + "/bsdl/template/pattern.head"):
        with open(project_path + "/bsdl/template/pattern.head") as f:
            for line in f:
                if re.search(r'SET_DEC_FILE', line):
                    dec_filename = line.split('"')[1]
                    print 'dec file name from pattern.head: ', dec_filename, '\n'
    # 若没有pattern.head, 则从pat文件夹中dec文件名
    elif os.path.exists(project_path + '/pat'):
        for root, dirs, files in os.walk(project_path + '/pat'):
            for f in files:
                if f.endswith('.dec'):
                    dec_filename = f
                    print 'dec file name from /pat/: ', dec_filename, '\n'
    if not dec_filename:
        exit('Erro: No dec, exit!')


    # 获取主pln的文件名，与工程目录名相同，后缀.pln
    mainpln_name = project_path.split('\\')[-1] + '.pln'

    # 获取工程目录下所有的.h文件名，会自动include在主pln中。注意只要在工程目录下的.h文件，都会被include
    include_files = list()
    for root, dirs, files in os.walk(project_path):
        for f in files:
            if root == project_path and f.lower().endswith('.h'):
                include_files.append(f)
    print "Below files will be included in main pln:"
    print '\n'.join(include_files)
    print '\n'

    # 将ip_lib文件夹中所有的pln文件名和txt文件名读入
    ip_plns = list()
    ip_txts = list()
    for root, dirs, files in os.walk(os.path.dirname(__file__) + '/ip_lib'):
        for f in files:
            if f.lower().endswith('.pln'):
                ip_plns.append(f)

            elif f.lower().endswith('.txt'):
                ip_txts.append(f)

    # 将item_lib文件夹中所有的txt文件名读入
    item_txts = list()
    for root, dirs, files in os.walk(os.path.dirname(__file__) + '/item_lib'):
        for f in files:
            if f.lower().endswith('.txt'):
                item_txts.append(f)

    # part1_head
    part1_head = ('/*  ===================================== PROGRAM HEAD =================================\n'
                  '============================SINO WEALTH ELECTRONIC (SHANGHAI) LTD.==================\n'
                  '====================================================================================\n'
                  'TEST CHIP              : SS___(Type No. For CP program)\n'
                  'TEST MACHINE           : 3360(Tester Name)\n'
                  'TEST FLOW              : FT\n'
                  'TEST TIME              : 1 Sites :  s(Site Number:Test time)\n'
                  'VERSION                : S0(Program vesion)\n'
                  'MODULE PROGRAM VERSION : v0.0(Module program version)\n'
                  'DATE                   : 20xx/xx/xx(yyyy/mm/dd)\n'
                  'PROGRAMMER             : TEAM(Programmer name)\n'
                  'FILE NAME              : %s\n'
                  'HIGH LEVEL             : %s\n'
                  'TYPICAL LEVEL          : %s\n'
                  'LOW LEVEL              : %s\n'
                  'SCHMIT1                : vil=%s*VDD, vih=%s*VDD\n'
                  'SCHMIT2                : vil=%s*VDD, vih=%s*VDD\n'
                  'SCHMIT3                : vil=%s*VDD, vih=%s*VDD\n'
                  'VOUT1                  : vol=GND+%s, voh=VDD-%s, iol=%s, ioh=%s, vref=%s\n'
                  'VOUT2                  : vol=GND+%s, voh=VDD-%s, iol=%s, ioh=%s, vref=%s\n'
                  'VOUT3                  : vol=GND+%s, voh=VDD-%s, iol=%s, ioh=%s, vref=%s\n'
                  'VOUT4                  : vol=GND+%s, voh=VDD-%s, iol=%s, ioh=%s, vref=%s\n'
                  '*/\n\n'
                  '/*  ====================================================================================\n'
                  '=====================================Declare Device .dec File ======================\n'
                  '====================================================================================*/\n') \
                  % (mainpln_name, high_level, typical_level, low_level, str(schmit1[0]), str(schmit1[1]), \
                    str(schmit2[0]), str(schmit2[1]), str(schmit3[0]), str(schmit3[1]), \
                    str(vout1[0]), str(vout1[1]), vout1[2], vout1[3], vout1[4], \
                    str(vout2[0]), str(vout2[1]), vout2[2], vout2[3], vout2[4], \
                    str(vout3[0]), str(vout3[1]), vout3[2], vout3[3], vout3[4], \
                    str(vout4[0]), str(vout4[1]), vout4[2], vout4[3], vout4[4]) \
                 + 'SET_DEC_FILE  ".\PAT\%s"\n\n' % dec_filename \
                 + """
/*  ====================================================================================
=====================================Declare .h File ===============================
====================================================================================*/
#include <math.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

"""
    # 添加之前准备的include_files
    for include_file in include_files:
        part1_head += '#include "%s"\n' % include_file

    # part2_level
    part2_level = """
/*  ====================================================================================
=====================================Declare Level Name&Level No.===================
====================================================================================*/
int rst_lvl                                                   = 1;
"""
    # part3_hwbin
    part3_hwbin = """
/*  ====================================================================================
=====================================Define HW&SW BIN ==============================
====================================================================================*/
HW_BIN_DEF
{
    // The Defination of HW BIN of CP Program according to the Product BIN DEFINE RULE//
    all_pass                                                  =  1;
"""
    part3_tail = """
    hw_fail_ap                                                = !6;
    // Please add according to your need

}
"""

    # part4_swbin
    part4_swbin = """
SW_BIN_DEF
{
    // The Defination of SW BIN of CP Program according to the your need,But Do not exceed 32//
    // The Defination of SW BIN of FT Program according to the Product BIN DEFINE RULE//
    all_pass                                                  =  1;
"""
    part4_tail = """
    sw_fail_ap                                                = !2;    
    // Please add according to your need

}
"""

    # part5_global
    part5_global = """
/*  ====================================================================================
=====================================Declare Global Variable =======================
====================================================================================*/

int  os_test_by_PMU[32];
GLOBAL int  debug_log_on=0;
"""

    # part6_randon
    part6_randon = """
/*=======================================================================================
                               for randon code generate
  =======================================================================================*/
FILE *fp_gcode;
int Table_id1[256]=
{
     0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
   157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
    35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
   190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
    70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
   219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
   101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
   248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
   140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
    17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
   175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
    50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
   202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
    87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
   233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
   116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53
} ;

int Table_id2[256]=
{
    93, 254, 160, 225, 191, 192, 158,  29,  67, 161, 255, 233, 183,  85,  11, 136,
   202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
    87,   9, 235, 181,  54, 117, 151, 201,  74,  20, 246, 168, 214,  52, 106,  43,
   104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22, 240, 174,  76,  18, 145,
    63, 221, 131, 194, 156, 126,   0,  94, 188, 226,  97,  32, 163, 253,  31,  65,
   157, 195,  33, 190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 127, 252, 162,
    64,  30,  95,   1, 227, 189,  62,  96, 130, 220,  58, 100, 134, 216,  91,   5,
   231, 185, 140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113,
   147, 205,  17,  79, 173, 243, 112,  46, 204, 146, 211, 175, 241,  19,  77, 206,
   144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238, 141, 111,  49, 178, 236,
    14,  80,  50, 108, 142, 208,  83,  13, 239, 177, 207,  45, 115,  75, 169, 247,
   116,  42, 200, 150,  21,  53, 137, 107,  70,  24, 250, 164,  39, 121, 155, 197,
   182, 232,  10,  84, 215,  71, 165, 251, 120,  38, 196, 154, 101,  59, 217, 135,
   132, 218,  56, 102, 229, 187,  89,   7, 248, 166,  68,  26, 153, 199,  37, 123,
   219, 133, 103,  57, 186, 228,   6,  88,  25,   4,  90, 184, 230, 167, 249,  27,
    69, 198, 152, 122,  36,   3, 128, 222,  60,  98,  35, 125, 159, 193,  66,  28
} ;

int Table_id3[256]=
{
    54, 214,  52, 106,  43, 179,  42, 200, 150,  21,  53, 117, 151, 201,  74,  20,
    41, 132, 218,  56, 102, 229, 187,  89, 119, 244, 170,  72,  22, 240, 174,  76,
    18, 145, 165, 251, 120,  38, 196, 154, 101,  59, 217, 135,  63, 221, 131, 194,
   208, 144, 114,  44, 109,  51, 209, 143,  12,  87,   9, 235, 181,  83,  13, 239,
   177, 207,  45, 115,  75, 169, 247,   7, 248, 166,  68,  26, 153, 199,  37, 123,
   246, 168, 137, 107,  70,  24, 250, 164,  39, 121, 155, 197,  60,  98,  35, 125,
   159, 193,  66,  14,  80,  50, 108, 142,  79, 173, 243, 112, 220,  58, 100, 134,
   156, 126,   0,  94, 188, 226,  97,  32, 163, 253,  31,  65, 216,  91,   5, 140,
   157, 195,  33, 190, 224,   2,  92, 223, 116, 231, 185, 129,  82, 176, 238, 141,
   111,  49, 178, 236,  99,  61, 124,  34, 127, 252, 162, 210,  48, 110,  46, 182,
    81,  15,  78,  16, 242, 189,  62,  96, 130, 147, 205,  17, 104, 172,  47, 113,
    19,  77, 206,  64,  30, 204, 146, 211, 175, 241, 237,  95,   1, 227,   6,  88,
   232,  10,  84, 215,  71,  25,   4, 191, 192, 158,  29,  67, 161,  57, 186, 228,
   255, 233, 183,  85,  11, 136,  90, 184, 230, 138, 212, 149, 203, 152, 122,  36,
     3, 128, 222, 167, 249,  27,  86, 180, 234, 202, 148, 118,  40, 171, 245, 105,
    55, 103,  69, 198,  28, 213, 139,  23,  73,   8,  93, 254, 160, 225, 219, 133
} ;

struct num_int
{
   int    ra_int : 30;
}random_temp;

struct idx_char
{
   int    ra_char : 30;
}ID00,ID11,ID22,ID33,Random_h,Random_l;

int index0,index1,index2,index3;
int rand0[32],rand1[32],rand2[32],rand3[32];
long int counter_i;

"""

    # part7_start_up
    part7_start_up = """
/*  ====================================================================================
=====================================START UP() BLOCK===============================
====================================================================================*/
START_UP()
{
"""
    part7_start_up_lotid = ''
    # 如果有flash_lotid则添加
    if flash_lotid == True:
        for root, dirs, files in os.walk(os.path.dirname(__file__)):
            for fl in files:
                if fl[:14] == 'flash_lotid_ip' and fl.endswith('.txt'):
                    with open(root + '\\' + fl) as f:
                        for line in f:
                            # 会替换dec_file和patname
                            if re.search(r'dec_file', line):
                                line = re.sub(r'dec_file,""', 'dec_file,"%s"' % dec_filename, line)
                            elif re.search(r'testcode', line):
                                line = re.sub(r'testcode,""', 'testcode,"%s"' % mainpln_name.split('.')[0], line)
                            elif re.search(r'xxx_pat', line):
                                line = re.sub(r'xxx', dec_filename.split('.')[0], line)
                            part7_start_up_lotid += line
        if not part7_start_up_lotid:
            exit("no flash_lotid!")

    part7_start_up_lotid += """
    //=========================Copy Patterns of code&lotid program End======================//
    """ + re.sub(r'xxx', dec_filename.split('.')[0], 'LOAD_PAT("./PAT/xxx_pat.ppo");')

    part7_start_up_level_head = """
    /*  ====================================Level Setting Start============================
    =========SET_LEVELN(lvl_no,pin_name,vil,vih,vol,voh,iol,ioh,vref,,);===============
    ===================================================================================*/
    SET_LEVELN(rst_lvl,IO_ALLPINS,0.0V,0.5V,0.0V,0.0V,0.0mA,0.0mA,0.0V,,);
    LEVELS(rst_lvl,6mS);

"""
    part7_start_up_timing = """
    /*======================================Level Setting End===============================*/
    /*=========== MARKERN(ts_no, pin_name, DT0, DT1, DT2, DT3, DT4, CT0, CT1 );=============*/
    /*======================================Timing Setting Start============================
    RATEN(time_set_no,period_value);
    MARKERN(time_set_no,ALLPINS,DT0,DT1,DT2,DT3,DT4,CT0,CT1);
    MARKERN(time_set_no,CLK_PIN,DT0,DT1,DT2,DT3,DT4,CT0,CT1);
    //Please add according to your need
    =========================================================================================*/
    
    RATEN(TS3,100nS);                                     //write mode or burn code
    MARKERN(TS3,IO_PINS, 0nS,  -1,-1,0nS,-1, 0nS,-1);
    
    RATEN(TS5,100nS);                                     //20M
    MARKERN(TS5,IO_PINS, 0ns,   -1,-1,0ns,-1,30nS,-1);
    MARKERN(TS5,tck, 20ns,   70nS,-1,0ns,-1,30nS,-1);
    
    RATEN(TS6,100nS);                                     //20M
    MARKERN(TS6,IO_PINS, 0ns,   -1,-1,0ns,-1,30nS,-1);
    MARKERN(TS6,SHADOW_tck, 20ns,   70nS,-1,0ns,-1,30nS,-1);
    
    RATEN(TS7,100nS);                                     //20M
    MARKERN(TS7,IO_PINS, 0ns,   -1,-1,0ns,-1,30nS,-1);
    MARKERN(TS7,tck+SHADOW_tck, 20ns,   70nS,-1,0ns,-1,30nS,-1);
    
    RATEN(TS14,100nS);                                 //for testmode
    MARKERN(TS14,IO_PINS,0nS,-1,-1,0nS,-1,100nS,-1);
    
    
    /*======================================Timing Setting End==============================*/
}
"""

    part_8 = """
/*  ====================================================================================
==================TEST START() BLOCK,YOU CANNOT MODIFY THIS BLOCK===================
====================================================================================*/
test_start()
{
    SOCKET_INC(FRZ_OFF)
    {
        os_test_by_PMU[SOCKET] = 0;
    }

    LEVELS(rst_lvl,1mS);

    FORCE_V_PMU(PMU,0V,@6V,@100mA,100mA,OFF,1mS);//PMU INITIALIZE

    /*================= mulit site=========================
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,ON,1mS);
    WAIT(10mS);
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,OFF,1mS);//PREF INITIALIZE(3360P2 Have not PREF)
    =====================================================*/

    FORCE_V_DPS(DPS_ALLPINS,0V,@8V,@1A,800mA,NORM,ON,1mS);
    WAIT(10mS);
    FORCE_V_DPS(DPS_ALLPINS,0V,@8V,@1A,800mA,NORM,OFF,1mS);//DPS INITIALIZE

    SET_CBIT(UR_ALLPINS,OFF,1mS);
    RELAY_OFF(IO_ALLPINS,ALL,1mS);
}


/*  ====================================================================================
==================TEST END() BLOCK,YOU CANNOT MODIFY THIS BLOCK=====================
====================================================================================*/
test_end()
{
    LEVELS(rst_lvl,0mS);

    FORCE_V_PMU(PMU,0V,@6V,@100mA,100mA,OFF,0mS);//PMU INITIALIZE

    /*================= mulit site=========================
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,ON,1mS);
    WAIT(10mS);
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,OFF,1mS);//PREF INITIALIZE(3360P2 Have not PREF)
    =====================================================*/

    FORCE_V_DPS(DPS_ALLPINS,0.0V,@8V,@2A,2A,NORM,ON,0mS);
    WAIT(10mS);
    FORCE_V_DPS(DPS_ALLPINS,0V,@8V,@1A,800mA,NORM,OFF,1mS);//DPS INITIALIZE

    SET_CBIT(UR_ALLPINS,OFF,1mS);
    RELAY_OFF(IO_ALLPINS,ALL,1mS);
}


/*  ====================================================================================
================= POWER_DWON() BLOCK,YOU CANNOT MODIFY THIS BLOCK===================
====================================================================================*/
power_down()
{
    LEVELS(rst_lvl,0mS);

    FORCE_V_PMU(PMU,0V,@6V,@100mA,100mA,OFF,0mS);//PMU INITIALIZE

    /*================= mulit site=========================
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,ON,1mS);
    WAIT(10mS);
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,OFF,1mS);//PREF INITIALIZE(3360P2 Have not PREF)
    =====================================================*/

    FORCE_V_DPS(DPS_ALLPINS,0.0V,@8V,@2A,2A,NORM,ON,0mS);
    WAIT(200mS);
    FORCE_V_DPS(DPS_ALLPINS,0V,@8V,@1A,800mA,NORM,OFF,1mS);//DPS INITIALIZE

    SET_CBIT(UR_ALLPINS,OFF,1mS);
    RELAY_OFF(IO_ALLPINS,ALL,1mS);
}


/*  ====================================================================================
===========POWER_DWON_FAIL_SITE() BLOCK,YOU CANNOT MODIFY THIS BLOCK================
====================================================================================*/
POWER_DOWN_FAIL_SITE()
{
    LEVELS(rst_lvl,0mS);


    //  FORCE_V_PMU(PMU,0V,@6V,@100mA,100mA,OFF,1mS);//PMU INITIALIZE

    /*================= mulit site=========================
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,ON,1mS);
    WAIT(10mS);
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,OFF,1mS);//PREF INITIALIZE(3360P2 Have not PREF)
    =====================================================*/

    FORCE_V_DPS(DPS_ALLPINS,0.0V,@8V,@2A,2A,NORM,ON,0mS);
    // WAIT(0mS);
    FORCE_V_DPS(DPS_ALLPINS,0V,@8V,@1A,800mA,NORM,OFF,0mS);//DPS INITIALIZE

    SET_CBIT(UR_ALLPINS,OFF,0mS);
    RELAY_OFF(IO_ALLPINS,ALL,0mS);
}

/*  ====================================================================================
==================RELEASE() BLOCK,YOU CANNOT MODIFY THIS BLOCK======================
====================================================================================*/
RELEASE()
{
    printf("-------IN RELEASE FILE---END-------------\\n");

    LEVELS(rst_lvl,1mS);

    FORCE_V_PMU(PMU,0V,@6V,@100mA,100mA,OFF,1mS);//PMU INITIALIZE

    /*================= mulit site=========================
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,ON,1mS);
    WAIT(10mS);
    FORCE_V_PREF(PREF_ALLPINS,0V,@12V,@100mA,100mA,NORM,OFF,1mS);//PREF INITIALIZE(3360P2 Have not PREF)
    =====================================================*/

    FORCE_V_DPS(DPS_ALLPINS,0.0V,@8V,@2A,2A,NORM,ON,0mS);
    WAIT(10mS);
    FORCE_V_DPS(DPS_ALLPINS,0V,@8V,@1A,800mA,NORM,OFF,1mS);//DPS INITIALIZE

    SET_CBIT(UR_ALLPINS,OFF,1mS);
    RELAY_OFF(IO_ALLPINS,ALL,1mS);
}

testmode_setting()
{
    // SET_CBIT(all_REs,ON,0mS);

    RELAY_ON(IO_PINS,PDCL,0mS);
    // FORCE_V_DPS(all_Powers,3.3V,@8V,@100mA,100mA,NORM,ON,1mS);
    LEVELS(fun_3v3_lvl,5mS);

    PIN_MODE(IO_PINS,NRZ,EDGE,ENABLE,IO_NRZ,OFF);
    PIN_MODE(MASK_PINS,NRZ,EDGE,MASK,IO_NRZ,OFF);

    // the sequence of the setting can not be modified.
}
"""

    part9_test_pro_head = """
TEST_PRO
{
    BEFORE_TEST : test_start;
    /*test item                                 ? pass_do :fail_do; */
"""

    part9_test_pro_tail= """
    allpass                ?#C(all_pass,all_pass) :#C(hw_fail_ap                                                ,sw_fail_ap                                                );
    AFTER_TEST : test_end;
}

allpass()
{
}

"""

    part10_function_bloc = """
/*  ====================================================================================
================================FUNCTION_TEST() BLOCK===============================
====================================================================================*/
"""

    level_set = set()
    level_set.add('rst_lvl')
    PAT_set = set()
    for i in range(len(item_list)):
        # 遍历item_list，找到item对应的txt文件
        if item_list[i] + '.txt' in item_txts:
            print 'hit item!', item_list[i]
            txt_file = os.path.dirname(__file__) + '\\item_lib\\' + item_list[i] + '.txt'

        elif item_list[i][:-5] + '.txt' in ip_txts:
            print 'hit ip!', item_list[i]
            for root, dirs, files in os.walk(os.path.dirname(__file__)):
                for fl in files:
                    if fl.split('.')[0] == item_list[i][:-5]:
                        txt_file = root + '\\' + fl
        else:
            print 'not hit, add enpty item:', item_list[i]
            txt_file = ''

        # txt存在，执行拼装
        if txt_file:
            with open(txt_file) as item_txt_file:
                for line in item_txt_file:
                    # LEVELS，记录在level_set中
                    if line.strip().startswith('LEVELS'):
                        level_set.add(line.split('(')[1].split(',')[0])
                    # TEST_NO，换号码
                    elif line.strip().startswith('TEST_NO'):
                        new_number = str(int(line.split(')')[0].split('(')[1]) + i * 100)
                        line = re.sub(r'\d+', new_number, line)
                    # DPS，替换参数
                    elif re.search(r'FORCE_V_DPS', line):
                        line = re.sub(r'high', high_level, line)
                        line = re.sub(r'typical', typical_level, line)
                        line = re.sub(r'low', low_level, line)
                    elif re.search(r'JUDGE_PAT', line):
                        PAT_set.add(line.split('(')[1].split(',')[0].split('+')[0])
                    part10_function_bloc += line
            part10_function_bloc += '\n\n\n'
        # txt不存在，打印一个空的项目
        else:
            part10_function_bloc += '%s()\n{\n    //add code here\n}\n\n' % item_list[i]

        # 分配hwbin号
        # 注意此处要求item名长度不得超过空格数量
        kong = '                                                  = !'
        part3_hwbin += '    hw_fail_' + item_list[i] + kong[len(item_list[i]):]
        if re.search(r'continuity', item_list[i]):
            part3_hwbin += '5;\n'
        else:
            part3_hwbin += '6;\n'

        # 分配swbin号
        # 注意此处要求item名长度不得超过空格数量
        part4_swbin += '    sw_fail_' + item_list[i] + kong[len(item_list[i]):]
        if re.search(r'continuity', item_list[i]):
            part4_swbin += '8;\n'
        else:
            part4_swbin += '2;\n'
        
        # 分配pass do/ fail do
        # 注意此处要求item名不超过（空格长度-len('hw_fail_')）个字符
        part9_test_pro_head += '    '
        kong = list()
        kong.append('                                            ? :#C(hw_fail_')
        kong.append('                                                  ,sw_fail_')
        kong.append('                                                  );\n')
        for k in kong:
            part9_test_pro_head += item_list[i] + k[len(item_list[i]):]

    # 分配LEVELN与level号
    # 注意此处要求level名不超过空格数量
    kong = '                                                          = '
    comment ='    /*==============================lvl_no==============================================*/\n'
    LEVELN = '    SET_LEVELN(lvl_no, pin_name, vil, vih, vol, voh, iol, ioh, vref);'
    level_set.remove('rst_lvl')
    level_list = list(level_set)
    level_list.sort()
    for i in range(len(level_list)):
        part2_level += 'int ' + level_list[i] + kong[len(level_list[i]):] + str(i+2) + ';\n'

        # 首先判断lvl名中是否有关键词
        special_lvl = False
        LEVELN_list = list()
        LEVELN_1 = ''
        LEVELN_2 = ''
        lns = level_list[i].split('_')
        for keyword in lvl_keywords:
            if keyword in lns:
                special_lvl = True
                break 
        # lvl名中有关键词，做特殊处理
        if special_lvl:
            lvlstr = '0.0V'
            if 'high' in lns:
                lvlstr = high_level
            elif 'typical' in lns:
                lvlstr = typical_level
            elif 'low' in lns:
                lvlstr = low_level
            # 第一行，按照high，low，typical填
            LEVELN_1 = re.sub(r'lvl_no', level_list[i], LEVELN)
            LEVELN_1 = re.sub(r'vil', '0.0V', LEVELN_1)
            LEVELN_1 = re.sub(r'vih', lvlstr, LEVELN_1)
            LEVELN_1 = re.sub(r'vol', str(round(max(0, float(lvlstr[:-1])/3-0.1), 2))+'V', LEVELN_1)
            LEVELN_1 = re.sub(r'voh', str(round(min(float(lvlstr[:-1]), float(lvlstr[:-1])/3+0.1), 2))+'V', LEVELN_1)
            LEVELN_1 = re.sub(r'io[h, l]', '0uA', LEVELN_1)
            LEVELN_1 = re.sub(r'vref', '0V', LEVELN_1)
            LEVELN_1 = re.sub(r'pin_name', 'IO_PINS', LEVELN_1)
            
            # 若有schmit或者vout，就有第二行
            smt = (0.0, 1.0)
            vot = (0.0, 0.0, '0uA', '0uA', '0V')
            pins = 'IO_PINS'
            if 'schmit' in map(lambda x: x[:-1], lns) or 'vout' in map(lambda x: x[:-1], lns) or 'normal' in lns or 'jtag' in lns:
                if 'schmit1' in lns:
                    smt = schmit1
                elif 'schmit2' in lns:
                    smt = schmit2
                elif 'schmit3' in lns:
                    smt = schmit3
                
                if 'vout1' in lns:
                    vot = vout1
                elif 'vout2' in lns:
                    vot = vout2    
                elif 'vout3' in lns:
                    vot = vout3    
                elif 'vout4' in lns:
                    vot = vout4    

                if 'jtag' in lns:
                    pins = 'JTAG_PINS'
                elif 'normal' in lns:
                    pins = 'IO_PINS-JTAG_PINS'
                LEVELN_2 = re.sub(r'lvl_no', level_list[i], LEVELN)
                LEVELN_2 = re.sub(r'vil', str(round(float(lvlstr[:-1])*smt[0], 2))+'V', LEVELN_2)
                LEVELN_2 = re.sub(r'vih', str(round(float(lvlstr[:-1])*smt[1], 2))+'V', LEVELN_2)
                if vot[0]:
                    LEVELN_2 = re.sub(r'vol', str(vot[0])+'V', LEVELN_2)
                else:
                    LEVELN_2 = re.sub(r'vol', str(round(max(0, float(lvlstr[:-1])/3-0.1), 2))+'V', LEVELN_2)
                if vot[1]:
                    LEVELN_2 = re.sub(r'voh', str(round(float(lvlstr[:-1])-vot[1], 2))+'V', LEVELN_2)
                else:
                    LEVELN_2 = re.sub(r'voh', str(round(min(float(lvlstr[:-1]), float(lvlstr[:-1])/3+0.1), 2))+'V', LEVELN_2)
                LEVELN_2 = re.sub(r'iol', vot[2], LEVELN_2)
                LEVELN_2 = re.sub(r'ioh', vot[3], LEVELN_2)
                LEVELN_2 = re.sub(r'vref', vot[4], LEVELN_2)
                LEVELN_2 = re.sub(r'pin_name', pins, LEVELN_2)
    
        # 无关键词的lvl，只写一行
        else:
            LEVELN_1 = re.sub(r'lvl_no', level_list[i], LEVELN)
            LEVELN_1 = re.sub(r'v[i,o][h,l]', '0.0V', LEVELN_1)
            LEVELN_1 = re.sub(r'io[h, l]', '0uA', LEVELN_1)
            LEVELN_1 = re.sub(r'vref', '0V', LEVELN_1)
            LEVELN_1 = re.sub(r'pin_name', 'IO_PINS', LEVELN_1)

        part7_start_up_level_head += re.sub(r'lvl_no', level_list[i], comment)
        part7_start_up_level_head += LEVELN_1 + '\n'
        if LEVELN_2:
            part7_start_up_level_head += LEVELN_2 + '\n'
        part7_start_up_level_head += '\n'

    # 写入主pln
    with open(project_path + '/' + mainpln_name, "w") as mpln:
        mpln.write(part1_head)
        mpln.write(part2_level)
        mpln.write(part3_hwbin)
        mpln.write(part3_tail)
        mpln.write(part4_swbin)
        mpln.write(part4_tail)
        mpln.write(part5_global)
        mpln.write(part6_randon)
        mpln.write(part7_start_up)
        mpln.write(part7_start_up_lotid)
        mpln.write(part7_start_up_level_head)
        mpln.write(part7_start_up_timing)
        mpln.write(part_8)
        mpln.write(part9_test_pro_head)
        mpln.write(part9_test_pro_tail)
        mpln.write(part10_function_bloc)

    # 显示并保存pat_list
    PAT_list = list(PAT_set)
    PAT_list.sort()
    print '\n\nBelow patterns are requied:'
    with open(project_path + '/pat_required.txt', "w") as pned:
        for pat in PAT_list:
            print pat
            pned.write(pat+'\n')
        print 'Above list is saved in pat_required.txt'


if __name__ == '__main__':
    main()