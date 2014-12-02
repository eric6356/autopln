//=====================================================//
//                 IP Version V00                      //
//=====================================================//

//======DE OPTION COMMON IP============================
   char verify_mass[10];
   char show_flag[10];  
   char which_one[10];  
   int  pat_interval;       
   double wait_time=0.0;
   double accurate_set; 
      
//------rc-ip-only-var-define--------------------------
   char active_static[10];         
   char freq_range[10];   
   double freq_target;    
   int rc_option_no;
   int rc_fir_option[32],rc_sec_option[32];
   int one_rc_ip_v00();
   int two_rc_ip_v00();     
               
//------ldo-ip-only-var-define-------------------------   
   double ldo_target;
   int ldo_option_no;
   double load_current=0.0;
   int ldo_fir_option[32],ldo_sec_option[32],ldo_thr_option[32];
   int one_ldo_ip_v00(); 
   int two_ldo_ip_v00();  
   int three_ldo_ip_v00();
          
//------lpd-ip-only-var--define------------------------
   double lpd_target;
   int lpd_option_no;
   int lpd_fir_option[32],lpd_sec_option[32],lpd_thr_option[32];
   int one_lpd_ip_v00();
   int two_lpd_ip_v00();
   int three_lpd_ip_v00();
   

//------LOTID-IP----------------------------------------
char startup_generate[15]; 
char call_lotid[15];       
char release_generate[15]; 


//------adc ip -----------------------------------------
long Nbit; 
long TSstep;
double adcstartvoltage,adcstopvoltage; 
double adcvref; 
char Capture_deep[10];
long Capture_line_perstep;
long MAX_bit_per_Capture_line;
int Channel_VREF; 
double ADC_level_VOH,ADC_level_VOL,ADC_level_VIH;  
char show_adc_data[10];
char show_analys_data[10];
double adc_inlh, adc_inll, adc_dnlh, adc_dnll,adc_offsetl, adc_offseth;
double adc_gain_errorl,adc_gain_errorh;
long valid_bit[20];   

int adc_normal_ip_v00();
int Adc_Valid_bit(int a,int b,int c,int d,int e ,int f,int g, int h,int i,int j,int k,int l,int m,int n,int o,int p,int q,int r,int s,int t);

//===================FLASH CONTROL IP===========
 
int ROM_SIZE;  //2K,4K,8K,16K,24K,32K,64K  
int E2_SIZE;  //0,1,2,3,4,5,6,7,8 ;//CP 8  FT 2
int IP_REAL_SIZE; //8 16 32 64
char flow[10];             
int ISP_SIZE;            //1,2,3,4


//------bias-ip-only-var-define-------------------------   
   double bias_target;
   int bias_option_no;
   double load_voltage=0.0;
   int one_bias_fir_option[32];
   int one_bias_adjust_ip_v00(); 
   
   
//------chopldo-ip-only-var-define-------------------------      
 int one_chopldo_fir_option[32];   
   int one_chopldo_ip_v00();   
   
   
//-------cmp-ip-define----------------------------------
   double cmp1_pmu_vin,cmp1_offset_spec,cmp1_P_vil,cmp1_P_vih,cmp1_P_vol,cmp1_P_voh,CMP1_R1,CMP1_R2; 
   int first_cmp_ip_v00(); 
   
   double cmp2_pmu_vin,cmp2_offset_spec,cmp2_P_vil,cmp2_P_vih,cmp2_P_vol,cmp2_P_voh,CMP2_R1,CMP2_R2; 
   int second_cmp_ip_v00(); 
   
   
//-------op-ip-define----------------------------------
double op1_P_vih,op1_P_vil,op1_P_voh,op1_P_vol;
double op2_P_vih,op2_P_vil,op2_P_voh,op2_P_vol;
double Vicm1,Vicm2,op1_out_pmu_current,op2_out_pmu_current;
double op1_Vio[32],op2_Vio[32];
double op1_out_pmu_vout[32],op1_N_pmu_vout[32],op1_P_pmu_vout[32]; 
double op2_out_pmu_vout[32],op2_N_pmu_vout[32],op2_P_pmu_vout[32];   
int one_op_ip_v00();
int two_op_ip_v00();   
   
   
   
   
   
   
   
   

//=====================================================//
//                 IP Version V01                      //
//=====================================================//



//------rc-ip-only-var-define--------------------------
   int one_rc_ip_v01();               
   int two_rc_ip_v01();               
   int rc_fir_tcon_change();         
   int rc_sec_tcon_change();         
   int rc_fir_tcon_change_return();  
   int rc_sec_tcon_change_return();  
   
//------ldo-ip-only-var-define-------------------------   
   int one_ldo_fir_option[32];
   int two_ldo_fir_option[32],two_ldo_sec_option[32];
   int thr_ldo_fir_option[32],thr_ldo_sec_option[32],thr_ldo_thr_option[32];
   int one_ldo_ip_v01(); 
   int two_ldo_ip_v01();  
   int three_ldo_ip_v01(); 
   char pdcl_rl_switch[10];    
   
   
//------lpd-ip-only-var--define------------------------
   int one_lpd_ip_v01();
   int two_lpd_ip_v01();
   int three_lpd_ip_v01();
   
  
//------ADC-ip-only-var--define------------------------   
   int adc_normal_ip_v01();
   
     
   
//------chopldo-ip-only-var-define-------------------------      
   int one_chopldo_adj_ip_v01();   
   
   
   
//-------cmp-ip-define----------------------------------  
   char ATE_TYPE[20];    
   int first_cmp_ip_v01(); 
   int second_cmp_ip_v01(); 
   
   
//-------op-ip-define----------------------------------
 
int one_op_ip_v01();
int two_op_ip_v01();
   
   








   
//=====================================================//
//                 IP Version V02                      //
//=====================================================// 

//------ldo-ip-only-var-define-------------------------
   double pre_load_current=0.0;
   double pre_wait_time=0.0;  
   int one_ldo_ip_v02();
   int two_ldo_ip_v02();
   int three_ldo_ip_v02();
   
//------lpd-ip-only-var--define------------------------
   int one_lpd_fir_option[32];
   int two_lpd_fir_option[32],two_lpd_sec_option[32];
   int thr_lpd_fir_option[32],thr_lpd_sec_option[32],thr_lpd_thr_option[32];
   int one_lpd_ip_v02();
   int two_lpd_ip_v02();
   int three_lpd_ip_v02();
      
//------rc-ip-only-var-define--------------------------
   int one_rc_adjust_ip_v02();
   int two_rc_adjust_ip_v02();
   int one_rc_ip_v02();
   int two_rc_ip_v02();
 
//------ADC-ip-only-var--define------------------------   
int pel_adc[190000];         //global       
double REFtargetV;
 
int adc_normal_calib_ip_v02();

//-------op-ip-define----------------------------------
 
int one_op_ip_v02();
int two_op_ip_v02();








//=====================================================//
//                 IP Version V03                      //
//=====================================================// 
//------lpd-ip-only-var--define------------------------
 
   int one_lpd_adjust_ip_v03();
   int two_lpd_adjust_ip_v03();
   int three_lpd_adjust_ip_v03();  
   
 

   
 //------ldo-ip-only-var-define-------------------------
 
   int one_ldo_ip_v03();
   int two_ldo_ip_v03();
   int three_ldo_ip_v03();
   
    
 //------ADC-ip-only-var--define------------------------     
   int ADCBUFFER;  
   double ADC_VDD[32];   
    
   int adc_normal_calib_ip_v03();   
   int adc_normal_calib_ip_v03_10bit();
   int adc_normal_calib_ip_v03_12bit();
   int Adc_Valid_bit_12bit(int a,int b,int c,int d,int e ,int f,int g, int h,int i,int j,int k,int l,int m,int n,int o,int p,int q,int r,int s,int t);
   int Adc_Valid_bit_10bit(int a,int b,int c,int d,int e ,int f,int g, int h,int i,int j,int k,int l,int m,int n,int o,int p,int q,int r,int s,int t);

   int adc_current_test();
   int REALY_ADC_GND_START();
   int REALY_ADC_GND_END(); 
   int REALY_ADC_VREF_START();     
   int REALY_ADC_VREF_END();       
   int REALY_ADC_CH_START();
   int REALY_ADC_CH_END();         
   

    

  
//=====================================================//
//                 IP Version V04                      //
//=====================================================// 

//------ldo-ip-only-var-define-------------------------   
     int one_ldo_adjust_ip_v04();
     int two_ldo_adjust_ip_v04();
     int three_ldo_adjust_ip_v04();
     
     
     
 //------ADC-ip-only-var--define------------------------     
       
   int first_adc_normal_calib_ip_v04();
   int second_adc_normal_calib_ip_v04();
   
   int Adc_Valid_bit_first(int a,int b,int c,int d,int e ,int f,int g, int h,int i,int j,int k,int l,int m,int n,int o,int p,int q,int r,int s,int t);
   int Adc_Valid_bit_second(int a,int b,int c,int d,int e ,int f,int g, int h,int i,int j,int k,int l,int m,int n,int o,int p,int q,int r,int s,int t);

      
     