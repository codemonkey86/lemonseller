#!/usr/bin/evn ruby

# Eventual intent of this script for TSP analysis
# is to loop over 30 runs for each of 6,7...11 cities 
# tabulating the memory, time, and in bound or not for each run
# also plan to add an average at each city count for these values
# Regression analysis can then be performed over averages
# or in detail for a set city size to see varience.

# To run right now:   prompt>   ruby analysis.rb


# output should be somethign similar to:
# [[6, "0.32802"]]
# [[6, ["3,224", 192.0]]]
# [[6, true]]

#  The idea is each tuple is [numcities, value]
# where value is either time,  memory tuple (stack, heap) or bounds boolean

# Intent is if you'd do each city count in a loop for say 30 times,  first 30 array tuples are the runs of 6 cities, then next 30 for 7, then so on.
# These could be easily parsed further as needed to do the regression analysis/writeup

numcities = 6
time = []
memory = []
bounds = []
  run  = `valgrind --tool=massif --stacks=yes ./a.out #{numcities} 3`
	  size = run.match(/Tour within bounds: yes/)
	  #pushing tuples of (city_count, value) where value
	  # is T/F,  double and long for bounds, time and memory respectively
	  bounds.push([numcities, !run.match(/Tour within bounds: yes/).nil?])
	  time.push([numcities, run.match(/RUN TIME:(.*)/)[1]])

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
 	
	   memory.push([numcities, [stack,heap1.to_f + heap2.to_f]])


puts time.inspect
puts memory.inspect
puts bounds.inspect



