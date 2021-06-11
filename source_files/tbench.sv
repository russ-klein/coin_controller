
/*
module test_driver (
   input logic       clock,
   input logic       resetn,

   output logic [10:0] purchase_msg,
   output logic        purchase_val,
   input logic         purchase_rdy,

   output logic [2:0]  coin_msg,
   output logic        coin_val,
   input logic         coin_rdy,

   input logic         dispense_msg,
   input logic         dispense_val,
   output logic        dispense_rdy,

   input logic  [2:0]  change_msg,
   input logic         change_val,
   output logic        change_rdy
);
*/

`define WIDTH 16
`define INT_BITS 5
`define FRAC_BITS (`WIDTH-`INT_BITS)

`define COIN_WIDTH 3

module test_driver (
   input        clock,
   input        resetn,

   output  [`WIDTH-1:0]        purchase_msg,
   output                      purchase_val,
   input                       purchase_rdy,

   output  [`COIN_WIDTH-1:0]   coin_msg,
   output                      coin_val,
   input                       coin_rdy,

   input                       dispense_msg,
   input                       dispense_val,
   output                      dispense_rdy,

   input   [`COIN_WIDTH-1:0]   change_msg,
   input                       change_val,
   output                      change_rdy
);

parameter too_long = 1024;

   real purchase_price;
   real change_due;
   real coin_values [0:5] = { 0.00, 0.05, 0.10, 0.25, 0.50, 1.00 };
   real amount_deposited;
   reg [`COIN_WIDTH-1:0] coin;

   reg [`WIDTH-1:0] purchase_msg_reg;
   reg        purchase_val_reg;
   reg [`COIN_WIDTH-1:0]  coin_msg_reg;
   reg        coin_val_reg;
   reg        dispense_rdy_reg;
   reg        change_rdy_reg;

   assign purchase_msg = purchase_msg_reg;
   assign purchase_val = purchase_val_reg;
   assign coin_msg = coin_msg_reg;
   assign coin_val = coin_val_reg;
   assign dispense_rdy = dispense_rdy_reg;
   assign change_rdy = change_rdy_reg;

   function [`WIDTH-1:0] to_ac_fixed;
     input real amount;
     
     automatic real adjusted_amount = amount * (1 << `FRAC_BITS);
     to_ac_fixed = adjusted_amount;
   endfunction

   task purchase;
      input real amount;

      purchase_msg_reg = to_ac_fixed(amount);
      purchase_val_reg = 1'b1;
      
      while (!purchase_rdy) @(posedge clock);
      @(posedge clock);
      purchase_val_reg = 1'b0;
   endtask

   task deposit_coin;
      input [`COIN_WIDTH:0] coin;

      coin_msg_reg = coin;
      coin_val_reg = 1'b1;

      while (!coin_rdy) @(posedge clock);
      @(posedge clock);
      coin_val_reg = 1'b0;
   endtask

   task deposit_random_coins;
      amount_deposited = 0.0;

      while (amount_deposited < purchase_price) begin
         coin = $urandom_range(5,1);
         amount_deposited = amount_deposited + coin_values[coin];
         deposit_coin(coin);
      end
      change_due = amount_deposited - purchase_price;
   endtask

   task get_product;

      static integer dispense_wait = 0;
      dispense_rdy_reg = 1'b1;
   
      while (!dispense_val) begin
         @(posedge clock);
         dispense_wait = dispense_wait + 1;
         if (dispense_wait > too_long) begin
            $display("where's my pop!!");
            $display("FAIL!");
            $finish;
         end
      end
      @(posedge clock);
      dispense_rdy_reg = 1'b0;   
   endtask
      
   task get_change;
      input real amount;
      static integer change_wait = 0;
      static real change_received = 0.0;

      change_rdy_reg = 1'b1;
   
      while (change_received != amount) begin
         while (!change_val) begin
            @(posedge clock);
            change_wait = change_wait + 1;
            if (change_wait > too_long) begin
               $display("where's my change!!");
               $display("I got ", change_received);
               $display("I was expecting ", change_due);
               $display("FAIL!");
               $finish;
            end
         end
         @(posedge clock);
         change_received = change_received + coin_values[change_msg];
      end
      change_rdy_reg = 1'b0;      
   endtask

   task wait_for_reset;
      while (!resetn) @(posedge clock); 
   endtask

   task wait_a_bit;
      automatic integer i;
      for (i=0; i<10; i+=1) @(posedge clock);
   endtask

   initial begin

      purchase_val_reg = 1'b0;
      coin_val_reg = 1'b0;
      dispense_rdy_reg = 1'b0;
      change_rdy_reg = 1'b0;

      wait_for_reset();
      wait_a_bit();

      purchase_price = $urandom_range(100,10) * 0.05;

      purchase(purchase_price);
      wait_a_bit();
      deposit_random_coins();
      wait_a_bit();
      get_product();
      wait_a_bit();
      get_change(change_due);

      $display("I got my pop!");
      $display("PASS!! ");
      $finish;
   end
endmodule

module tbench;

   parameter CLOCK_PERIOD = 1000;
   parameter RESET_PERIOD = 10000;

   logic clock;
   logic resetn;

   wire [`WIDTH-1:0]         purchase_msg;
   wire                      purchase_val;
   wire                      purchase_rdy;
   wire [`COIN_WIDTH-1:0]    coin_msg;
   wire                      coin_val;
   wire                      coin_rdy;
   wire                      dispense_msg;
   wire                      dispense_val;
   wire                      dispense_rdy;
   wire [`COIN_WIDTH-1:0]    change_msg;
   wire                      change_val;
   wire                      change_rdy;

   initial clock = 1'b1;
   always  #CLOCK_PERIOD clock = ~clock;
      
   initial begin
      resetn = 1'b0;
      #RESET_PERIOD resetn = 1'b1;
   end

   coin_counter_wrapper cc1(
      .clock        (clock),
      .resetn       (resetn),

      .purchase_msg (purchase_msg),
      .purchase_val (purchase_val),
      .purchase_rdy (purchase_rdy),
      .coin_msg     (coin_msg),
      .coin_val     (coin_val),
      .coin_rdy     (coin_rdy),
      .dispense_msg (dispense_msg),
      .dispense_val (dispense_val),
      .dispense_rdy (dispense_rdy),
      .change_msg   (change_msg),
      .change_val   (change_val),
      .change_rdy   (change_rdy)
   );

   test_driver td1(
      .clock        (clock),
      .resetn       (resetn),

      .purchase_msg (purchase_msg),
      .purchase_val (purchase_val),
      .purchase_rdy (purchase_rdy),
      .coin_msg     (coin_msg),
      .coin_val     (coin_val),
      .coin_rdy     (coin_rdy),
      .dispense_msg (dispense_msg),
      .dispense_val (dispense_val),
      .dispense_rdy (dispense_rdy),
      .change_msg   (change_msg),
      .change_val   (change_val),
      .change_rdy   (change_rdy)
   );

endmodule
