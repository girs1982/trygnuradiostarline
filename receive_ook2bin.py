"""
Embedded Python Blocks:

Each this file is saved, GRC will instantiate the first class it finds to get          в секунде миллион микросекунд т.е делитель 2
ports and parameters of your block. The arguments to __init__  will be the
parameters. All of them are required to have default values!
"""
import time
import numpy as np
from gnuradio import gr
from datetime import datetime
class blk (gr.sync_block):
    """
    Block to decode the data on an already squared signal, comprised of 0's and 1's.
    """

    def __init__ (self
                , baseband_freq = 600
                , sample_rate = 2e6
                , sink_file = None):  # only default arguments here
        """
        Constructor.

        Args:
            baseband_freq -> Frequency of the baseband signal

            sample_rate -> Number of samples per second

            sink_file -> File to dump the packets. If it's 'None', prints them on STDOUT
        """
        gr.sync_block.__init__(
            self,
            name = 'OOK to bin sink',
            in_sig = [np.float32],
            out_sig = []
        )

        # Number of samples to discern long and short bursts
        #   sample_rate / baseband_freq = samples_per_period
        self.threshold = (sample_rate / baseband_freq) / 2

        self.sink_file = sink_file

        self.time_delta = 0 # Timer to measure the time between edges

        self.rising_timestamps = []
        self.falling_timestamps = []
        self.previous_sample = 0  #posl sig rx
        self.lastRxValue = 1
        self.Rxpin = 0  #posl sig rx

        # Counter to determine the end of a packet. If more than (2 * threshold) samples
        # are set to 0, the packet is done
        self.allzero_count = 0
        self.starline_counter_pre = 0
        self.starline_counter = 0
        self.staline_state = 0
        self.bValidPacket=False
        self.lastRxTime=0
        self.lastRx=0
        self.difTime=0
        self.tempTime=0
        self.packet = [] # List to store the bits of the packet
        self.starline_code= [0,0,0,0,0,0,0,0]
        self.state = 0
        self.sym_count = 0
        self.bit_count = 0
        self.byte = 0
        self.sym = 0
        self.packet = []        
        self.dt = datetime.now()
        self.mic=self.dt.microsecond

    def reset_state(self):
        self.state = 0
        self.sym_count = 0
        self.bit_count = 0
        self.byte = 0
        self.sym = 0
        self.packet = []
    def decodestarline (self) :
        if (self.difTime >900*2 and self.difTime <1100*2 and self.staline_state ==0  and self.lastRx==1):
           #print("starline preambule")
           self.starline_counter_pre +=1
           if self.starline_counter_pre == 6:
                print("starline preambule")
                self.staline_state = 1  
                # print(self.difTime)              
        # preambule good      
        elif (self.staline_state==1):
            if (self.difTime >350*2 and self.difTime <650*2 and self.lastRx==1): 
                self.packet.append ("0")
                self.bValidPacket = True
            elif (self.difTime >150*2 and self.difTime <350*2 and self.lastRx==1): 
                self.packet.append ("1")
                self.bValidPacket = True
            else: 
                self.staline_state = 1 
                self.starline_counter=0
            if(self.bValidPacket ==True):
               self.starline_counter+=1
               # print(self.starline_counter)
               if(self.starline_counter==64):
                  print("getcode")
                  self.starline_counter=0 
                  self.staline_state=0
                  self.time_delta=0
                  self.starline_counter_pre=0                           
                  #bin_strstar = str (self.packet)                                 
                 # print(self.packet)  
                  print(self.packet)                                
                  self.packet = []
                 



    def work (self, input_items, *args, **kwargs):
    
        
    
        samples = input_items [0]
        
        in0 = input_items[0]
        processed = 0    
        

        for i in range(0, len(input_items[0])):
            self.time_delta += 1
            self.Rxpin=round(input_items[0][i])
            if(self.Rxpin!=self.lastRx):
                self.tempTime=self.time_delta
                self.difTime=self.tempTime-self.lastRxTime
                # print(self.difTime)
                self.decodestarline()
                self.lastRxTime=self.tempTime
                self.lastRx=self.Rxpin
                
                
               
            
        
        # for sym in in0:
            # print(in0)
            
        #self.time_delta += len (samples)
        
        # if self.time_delta>1000000 and self.time_delta<2000005:
            # print(self.time_delta)
            # print(self.Rxpin)

                


        #self.consume(0, len(input_items[0]))     ///uvelichivaet kakto v hz   
        
        #print(self.time_delta)

        return len (samples)


