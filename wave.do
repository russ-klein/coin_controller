onerror resume
wave tags F0 
wave update off
wave add tbench.dispense_msg -tag F0 -radix hexadecimal
wave group tbench -backgroundcolor #004466
wave add -group tbench {tbench.CLOCK_PERIOD[31:0]} -tag F0 -radix hexadecimal
wave add -group tbench {tbench.RESET_PERIOD[31:0]} -tag F0 -radix hexadecimal
wave add -group tbench tbench.clock -tag F0 -radix hexadecimal
wave add -group tbench tbench.resetn -tag F0 -radix hexadecimal
wave add -group tbench {tbench.purchase_msg[10:0]} -tag F0 -radix hexadecimal
wave add -group tbench tbench.purchase_val -tag F0 -radix hexadecimal
wave add -group tbench tbench.purchase_rdy -tag F0 -radix hexadecimal
wave add -group tbench {tbench.coin_msg[2:0]} -tag F0 -radix hexadecimal
wave add -group tbench tbench.coin_rdy -tag F0 -radix hexadecimal
wave add -group tbench tbench.coin_val -tag F0 -radix hexadecimal
wave add -group tbench tbench.dispense_val -tag F0 -radix hexadecimal
wave add -group tbench tbench.dispense_rdy -tag F0 -radix hexadecimal
wave add -group tbench {tbench.change_msg[2:0]} -tag F0 -radix hexadecimal
wave add -group tbench tbench.change_val -tag F0 -radix hexadecimal
wave add -group tbench tbench.change_rdy -tag F0 -radix hexadecimal
wave insertion [expr [wave index insertpoint] + 1]
wave update on
wave top 0
wave zoom range 0 106000
