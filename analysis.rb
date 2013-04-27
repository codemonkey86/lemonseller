#!/usr/bin/evn ruby


# To run right now:   prompt>   ruby analysis.rb


# File format should be as follows for each city set two files are generated: 
# PERM-TSP-X.out and NAIVE-TSP-X.out  where X = num ciites

# each file has 1 line per trial :
#       bound(as 0/1) time stack heap
# and ends with an average line: 
#       bound% avg_time avg_stack avg_heap

# Based on valgrind, neither stack nor heap are guaranteed to be peak, but are guaranteed to have occured at approximately peak total memory consumption

#TODO  set up inner/outer loop,  File write method  test/debug
trials = 30
naive_time = []
naive_memory = []
naive_bounds = []


perm_time = []
perm_memory = []
perm_bounds = []



for x in 6..7
  numcities = x
  naive_time_sum =  0
  naive_bound_sum = 0
  naive_heap_sum = 0
  naive_stack_sum =0 
  perm_time_sum = 0
  perm_bound_sum = 0
  perm_heap_sum = 0
  perm_stack_sum =0 
  permf = File.new("PERM-TSP-#{numcities}.out", "wb")
   naivef = File.new("NAIVE-TSP-#{numcities}.out", "wb")
   for j in 1..trials 

	

	 
          #NAIVE
          `rm -rf massif.*`
          `rm -rf tmp`
          `rm -rf a.out`
          `g++ -g naive_tsp.cpp -lemon`
         
          
          run  = `valgrind --tool=massif --stacks=yes ./a.out #{numcities} 3`
	  size = run.match(/Tour within bounds: yes/)
	  
          # parse for, print to file, and accumulate bound information
          bound = !run.match(/Tour within bounds: yes/).nil?
          naivef.write("#{bound},")  #don't endl
          naive_bound_sum += 0 if !bound
          naive_bound_sum += 1 if bound
              
          #parse for, print to file, and accumulate time info
          time = run.match(/RUN TIME:(.*)/)[1]
          naivef.write("#{time},")  #no endl
          naive_time_sum += time.to_f

	  #find out which massif.out.xxxx file is from child thread that did the work
	  filenum = run.match(/PID:(.*)/)[1]
	   #pretty print this file into a tmp file
	  `ms_print massif.out.#{filenum} > tmp`
	  memfile= File.new("tmp", "rb")
	  memtext = memfile.read
	  #parse file to find out which snapshot had peak memory usage
	  peaksnapshot = memtext.match(/Detailed snapshots:.*?([0-9]+)\s\(peak.*/)[1]
          print peaksnapshot

	  #parse that snapshot for actual peak memory usage
	  heap1 = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?(\S+)/)[1]
	  heap2 =  memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1]
          stack = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1]
 	
	  #accumulate and print mem usage to file
          naivef.write("#{stack},") #no endl
          naivef.write("#{heap1.to_f+ heap2.to_f}") #endl
          naive_stack_sum += stack.to_f
          naive_heap_sum += heap1.to_f + heap2.to_f


 	#PERMANENT:
	 `rm -rf massif.*`
          `rm -rf tmp`
          `rm -rf a.out`
          `g++ -g permanent_tsp.cpp -lemon`
         
          
          run  = `valgrind --tool=massif --stacks=yes ./a.out #{numcities} 3`
	  size = run.match(/Tour within bounds: yes/)
	  
          # parse for, print to file, and accumulate bound information
          bound = !run.match(/Tour within bounds: yes/).nil?
          permf.write("#{bound},")  #don't endl
          perm_bound_sum += 1 if bound 
          perm_bound_sum += 0 if !bound
              
          #parse for, print to file, and accumulate time info
          time = run.match(/RUN TIME:(.*)/)[1]
          permf.write("#{time},") #no endl
          perm_time_sum += time

	  #find out which massif.out.xxxx file is from child thread that did the work
	  filenum = run.match(/PID:(.*)/)[1]
	   #pretty print this file into a tmp file
	  `ms_print massif.out.#{filenum} > tmp`
	  memfile= File.new("tmp", "rb")
	  memtext = memfile.read
	  #parse file to find out which snapshot had peak memory usage
	  peaksnapshot = memtext.match(/Detailed snapshots:.*?([0-9]+)\s\(peak.*/)[1]
          print peaksnapshot

	  #parse that snapshot for actual peak memory usage
	  heap1 = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?(\S+)/)[1]
	  heap2 =  memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1]
          stack = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1]
 	
	  #accumulate and print mem usage to file
          permf.write("#{stack},") #no endl
          permf.write("#{heap1.to_f+ heap2.to_f}") #endl
          perm_stack_sum += stack
          perm_heap_sum += heap1.to_f + heap2.to_f

        
         
        end # end of 30 iterations now get averages
       
       
     
         
        
        naivef.write("AVERAGES: #{naive_time_sum/trials},#{naive_stack_sum/trials},#{naive_heap_sum/trials},#{naive_bound_sum/trials}")
        permf.write("AVERAGES: #{perm_time_sum/trials},#{perm_stack_sum/trails},#{perm_heap_sum/trails},#{perm_bound_sum/trials}")
       
   end



#TODO: get permanent to naive output, then test away!


