#!/usr/bin/env python  

#
#fifo = open(path, "w")
#fifo.write("1 sender!\n")
#fifo.close()

  
import os
import sys;  
import getopt;  
  
def usage():  
    print("Usage: ./fifo_send.py [-f] [-b] [-l] [-r] [-s]");  
  
TYPE_PIPE_CMD = {'FORWARD':1, 'BACKWARD':2, 'LEFT':3, 'RIGHT':4, 'STOP':0} 

PATH_PIPE = "/tmp/kr_fifo"

str_cmd = 'FORWARD'

if "__main__" == __name__:  
#    os.mkfifo(PATH_PIPE)

    fifo = open(PATH_PIPE, "w")

    try:  
        opts,args = getopt.getopt(sys.argv[1:],\
		"fbl:r:h", ["forward","backward", "left", "right", "help"]);  
      
        print("============ opts ==================");         
        print(opts);  
      
        print("============ args ==================");  
        print(args);  
          
        #check all param  
        for opt,arg in opts:
            if opt in ("-h", "--help"):  
                usage();
                sys.exit(1);  
            elif opt in ("-f", "--forward"):  
                str_cmd = 'FORWARD'
            elif opt in ("-b", "--backward"):  
                str_cmd = 'BACKWARD'
            elif opt in ("-l", "--left"):  
                str_cmd = 'LEFT'
            elif opt in ("-r", "--right"):  
                str_cmd = 'RIGHT'
            else:  
                print("%s  ==> %s" %(opt, arg));          
          
    except getopt.GetoptError:  
        print("getopt error!");  
        usage();  
        sys.exit(1);  
	fifo.close()

    print("sending cmd: %c" % str_cmd)
    fifo.write("%c\n"%(str_cmd))
    fifo.close()
