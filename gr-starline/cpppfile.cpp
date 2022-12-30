/* -*- c++ -*- */
/*
 * Copyright 2022 gansta.
 *

 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "starline_decode_impl.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream> 

namespace gr {
  namespace starline {
    using input_type = float;
    using output_type = float;
    int starlinePreambula = 0;
    bool bValidPacket=false;
    int decodeMethod = 1;
    int lastRxValue = 0;
    int tempRxValue = 0;
    unsigned long lastRxTime = 0;
    unsigned long tempTime = 0; 
    int starlineZCounter = 0;
    int starlineZpreambulaCounter = 0;
    unsigned long difTime2 = 0;
    unsigned long difTime = 0;
	///std::byte starline_code[9]={0,0,0,0,0,0,0,0,0};
    int starline_state = 0;
    int starline_code[64];
    float *out2;
    int pream1=1800;
    int pream2=2200;
    int stl1=700;
    int stl2=1300;
    int sth1=300;
    int sth2=700;
    using namespace std;
    std::ostringstream os;
    starline_decode::sptr
    starline_decode::make()
    {
      return gnuradio::get_initial_sptr
        (new starline_decode_impl());
    }


    /*
     * The private constructor
     */
    starline_decode_impl::starline_decode_impl()
      : gr::block("starline_decode",
              gr::io_signature::make(1, 1, sizeof (float)), // Number and type of inputs
              gr::io_signature::make(1, 1, sizeof (float))) // Number and type of outputs
    {}

    /*
     * Our virtual destructor.
     */
    starline_decode_impl::~starline_decode_impl()
    {
    }
 
string GetHexFromBin(string sBinary)
{
	string rest("0x"),tmp,chr = "0000";
	int len = sBinary.length()/4;
	chr = chr.substr(0,len);
	sBinary = chr+sBinary;
	for (int i=0;i<sBinary.length();i+=4)
	{
		tmp = sBinary.substr(i,4);
		if (!tmp.compare("0000"))
		{
			rest = rest + "0";
		}
		else if (!tmp.compare("0001"))
		{
			rest = rest + "1";
		}
		else if (!tmp.compare("0010"))
		{
			rest = rest + "2";
		}
		else if (!tmp.compare("0011"))
		{
			rest = rest + "3";
		}
		else if (!tmp.compare("0100"))
		{
			rest = rest + "4";
		}
		else if (!tmp.compare("0101"))
		{
			rest = rest + "5";
		}
		else if (!tmp.compare("0110"))
		{
			rest = rest + "6";
		}
		else if (!tmp.compare("0111"))
		{
			rest = rest + "7";
		}
		else if (!tmp.compare("1000"))
		{
			rest = rest + "8";
		}
		else if (!tmp.compare("1001"))
		{
			rest = rest + "9";
		}
		else if (!tmp.compare("1010"))
		{
			rest = rest + "A";
		}
		else if (!tmp.compare("1011"))
		{
			rest = rest + "B";
		}
		else if (!tmp.compare("1100"))
		{
			rest = rest + "C";
		}
		else if (!tmp.compare("1101"))
		{
			rest = rest + "D";
		}
		else if (!tmp.compare("1110"))
		{
			rest = rest + "E";
		}
		else if (!tmp.compare("1111"))
		{
			rest = rest + "F";
		}
		else
		{
			continue;
		}
	}
	return rest;
}    
    
    
    
void starline_get(){
    bValidPacket = false;
    if(starline_state==0){//ждем преамбулу и хедер
      if(difTime2 > pream1 && difTime2 < pream2 && lastRxValue ==1 ){
        starlineZpreambulaCounter ++;
      } 
      if(starlineZpreambulaCounter==6){
            starline_state=1;   
            cout << "preambule"  << endl;      
        }

    }
    /////////////////////////////////////////////////////PREAMBULA STARA///////////////////////////////////////////////////////////////////////////
    else if(starline_state==1){	
	// получаем биты	
      if(difTime2 > stl1 && difTime2 < stl2 && lastRxValue == 1){// получили 1    
         
          starline_code[starlineZCounter]=0;
        bValidPacket = true;
      }//////////
      else if(difTime2 > sth1 && difTime2 < sth2 && lastRxValue == 1){  
      starline_code[starlineZCounter]=1;     
        bValidPacket = true;
      }////////////
      else if(lastRxValue == 0){
      }//////	 	  
      else{
        starline_state=1;
        starlineZCounter = 0;
      }      
      if(bValidPacket){
      ///out2[starlineZCounter] =starline_code[starlineZCounter];
        starlineZCounter++;
        if(starlineZCounter==64){           //64ili66
          //starline_vardump();
          starlineZCounter = 0;
          starline_state = 0;
          starlineZpreambulaCounter = 0; /////////novshetch
          ///starkk++;                   /////////shetchik
          cout << "getpacket"  << endl;
          for(int i:starline_code){ 
           
            os << i;
          }
           std::string str(os.str());
         // cout <<str<< endl;
           std::string str1 =GetHexFromBin(str);
           cout <<str1 <<"\n"<< endl;
           str="";
           str1=""; 
           os.str("");
;        
          
          tempTime=0;
        }
      }
    }
}
    void
    starline_decode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
     ninput_items_required[0] = noutput_items;
    }

    int
    starline_decode_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
   float *in = (float *) input_items[0];
    float *out = (float *) output_items[0];
    
      // Do <+signal processing+>
     for(int i = 0; i < noutput_items; i++) {		  
		  tempTime++;	
		  tempTime=tempTime;	  
		  tempRxValue=(int)(in[i]);
		 /// out[i] = in[i] ;
		   		  
           if(tempRxValue != lastRxValue){
          /// cout << tempRxValue   << endl;///work right           
				                difTime2 = tempTime - lastRxTime;
						starline_get(); 
						// cout << difTime2  << endl;
						lastRxTime = tempTime;
						lastRxValue = tempRxValue; }  		
      
      }///close for 
	  
	  
	///  out=out2;
	  
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);
      // Tell runtime system how many output items we produced.
      return noutput_items;
     // return sizeof(in);
    }

  } /* namespace starline */
} /* namespace gr */
