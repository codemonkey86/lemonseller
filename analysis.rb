#!/usr/bin/evn ruby

#TODO: test run with 300 permanent,  why does valgrind segfault? worst case drop space/stack for 300
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



for x in ARGV[0]..ARGV[1]
  numcities = x
  naive_time_sum =  0
  naive_bound_sum = 0
  naive_heap_sum = 0
  naive_stack_sum =0 
  naive_cost_sum = 0
  perm_cost_sum = 0
  perm_time_sum = 0
  perm_bound_sum = 0
  perm_heap_sum = 0
  perm_stack_sum =0 
  permf = File.new("PERM-TSP-#{numcities}.out", "wb")
  if ARGV[2] == "1"
   naivef = File.new("NAIVE-TSP-#{numcities}.out", "wb")
  end
   for j in 1..trials 

	

	 
          #NAIVE
          if ARGV[2] == "1"
          `rm -rf massif.*`
          `rm -rf tmp`
          `rm -rf a.out`
          `g++ -g naive_tsp.cpp -lemon`
         
          puts "IN IF"
          run  = `valgrind --tool=massif --stacks=yes ./a.out #{numcities} 3`
	  size = run.match(/Tour within bounds: yes/)
	  
          # parse for, print to file, and accumulate bound information
          bound = !run.match(/Tour within bounds: yes/).nil?
          naivef.write("#{bound},")  #don't endl
          naive_bound_sum += 0 if !bound
          naive_bound_sum += 1 if bound

          cost = run.match(/The minimum tour length of this graph is\s([0-9]+)/)[1]
          naivef.write("#{cost},") 
          naive_cost_sum += cost.to_f

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
	  heap1 = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?(\S+)/)[1].gsub(",","").to_f
	  heap2 =  memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1].gsub(",","").to_f
          stack = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1].gsub(",","").to_f
 	
	  #accumulate and print mem usage to file
          naivef.write("#{stack},") #no endl
          puts "printing endl"
          naivef.write("#{heap1+ heap2}\n") #endl
          naive_stack_sum += stack
          naive_heap_sum += heap1 + heap2
  end

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

	  cost = run.match(/The minimum tour length of this graph is\s([0-9]+)/)[1]
          puts cost
          permf.write("#{cost},") 
          perm_cost_sum += cost.to_f
              
          #parse for, print to file, and accumulate time info
          time = run.match(/RUN TIME:(.*)/)[1]
          permf.write("#{time},") #no endl
          perm_time_sum += time.to_f

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
	  heap1 = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?(\S+)/)[1].gsub(",","").to_f
	  heap2 =  memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1].gsub(",","").to_f
          stack = memtext.match(/^\s*#{peaksnapshot}\s+?\S+?\s+?\S+?\s+?\S+?\s+?\S+?\s+?(\S+)/)[1].gsub(",","").to_f
 	
	  #accumulate and print mem usage to file
          permf.write("#{stack},") #no endl
          permf.write("#{heap1+ heap2}\n") #endl
          perm_stack_sum += stack
          perm_heap_sum += heap1+heap2

       
         
        end # end of 30 iterations now get averages
       
       if (ARGV[2] == "1")
        puts "naive sum" 
        puts  naive_stack_sum
         
        
        naivef.write("AVERAGES: #{naive_time_sum/j},#{naive_stack_sum/j},#{naive_heap_sum/j},#{naive_bound_sum.to_f/j.to_f},#{naive_cost_sum.to_f/j.to_f}")
      end 
       permf.write("AVERAGES: #{perm_time_sum/j},#{perm_stack_sum/j},#{perm_heap_sum/j},#{perm_bound_sum.to_f/j.to_f},#{perm_cost_sum.to_f/j.to_f}")
       
   end



#TODO: get permanent to naive output, then test away!


