
#include <systemc.h>
#include "coin_counter_wrapper.h"

#ifdef QUESTA
SC_MODULE_EXPORT(coin_counter_wrapper);
#endif


#ifdef OSCI

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TOO_LONG 400

SC_MODULE(systemc_testbench)
{
  sc_in<bool> clock;
  sc_in<bool> resetn;

  Connections::Out<money_t>  INIT_S1(purchase);
  Connections::Out<coin_t>   INIT_S1(coin);
  Connections::In<bool>      INIT_S1(dispense);
  Connections::In<coin_t>    INIT_S1(change_conn);

  const double value_array[] = { 0.0, 0.05, 0.10, 0.25, 0.50, 1.00 };

  SC_CTOR(systemc_testbench)
  {
     srand((unsigned)time(NULL));

     SC_THREAD(random_test);
     sensitive<<clock.pos();
     reset_signal_is(resetn, false);
  }

  random_int(int n)
  {
     // return int between 0 and n, inclusive
     return rand()%(n+1);
  }

  void random_test()
  {
     double purchase_price = (random_int(90) + 10) * 0.05;
     double amount_deposited = 0.0;
     double change_due;
     double change_received;
     int wait_count;
     bool got_my_pop = false;

     purchase.Reset();
     coin.Reset();
     dispense.Reset();
     change_coin.Reset();

     wait();
     while(1) {
       purchase.write(purchase_price);
       while (amount_deposited < purchase_price) {
          coin_t random_coin = random_int(4) + 1;
          coin.write(random_coin);
          amount_deposited += value_array[random_coin];
          wait();
       }
       wait_count = 0;
       while (! got_my_pop) {
          got_my_pop = dispense.nb_read(pop);
          wait_count++;
          if (wait > TOO_LONG) {
             printf("Where's my pop?!? \n");
             printf("FAIL! \n");
             sc_stop();
          }
       }
       change_due = amount_deposited - purchase_price;
       change_received = 0;
       wait_count = 0;
       while (change_due != change_received) {
          if (change.nb_read(change_coin)) {
             change_received += value_array[change_coin];
          }
          wait();
          wait_count++;
          if ((wait_count > TOO_LONG) || (change_received > change_due)) {
             printf("Where's my change?!? \n");
             printf("expected: %5.2f got: %5.2f \n", change_due, change_received);
             printf("FAIL! \n");
             sc_stop();
          }
       }
       printf("PASS! \n");
       sc_stop();
     }
   }
     
   void pay_with_nickels()
   {
     double purchase_price = 1.75
     double amount_deposited = 0.0;
     double change_due;
     double change_received;
     int wait_count;
     bool got_my_pop = false;

     purchase.Reset();
     coin.Reset();
     dispense.Reset();
     change_coin.Reset();

     wait();
     while(1) {
       purchase.write(purchase_price);
       while (amount_deposited < purchase_price) {
          coin_t my_coin = 1;
          coin.write(my_coin);
          amount_deposited += value_array[my_coin];
          wait();
       }
       wait_count = 0;
       while (!got_my_pop) {
          got_my_pop = dispense.nb_read(pop);
          wait_count++;
          if (wait > TOO_LONG) {
             printf("Where's my pop?!? \n");
             printf("FAIL! \n");
             sc_stop();
          }
       }
       change_due = amount_deposited - purchase_price;
       change_received = 0;
       wait_count = 0;
       while (change_due != change_received) {
          if (change.nb_read(change_coin)) {
             change_received += value_array[change_coin];
          }
          wait();
          wait_count++;
          if ((wait_count > TOO_LONG) || (change_received > change_due)) {
             printf("Where's my change?!? \n");
             printf("expected: %5.2f got: %5.2f \n", change_due, change_received);
             printf("FAIL! \n");
             sc_stop();
          }
       }
       printf("PASS! \n");
       sc_stop();
     }
  }
}

SC_MODULE(top)
{
  sc_clock clock;
  sc_signal<bool> resetn;

  Connections::Combinational<money_t>  INIT_S1(purchase);
  Connections::Combinational<coin_t>   INIT_S1(coin);
  Connections::Combinational<bool>     INIT_S1(dispense);
  Connections::Combinational<coin_t>   INIT_S1(change_conn);

  coin_counter_mc   INIT_S1(cc_inst);
  systemc_testbench INIT_S1(sctb);

  SC_CTOR(top) :
        clock("clock",10, SC_NS, 0.5, 0, SC_NS, true)
  {
     cc_inst.clock(clock);
     cc_inst.resetn(resetn);
     cc_inst.purchase(purchase);
     cc_inst.quarter(coin);
     cc_inst.dispense(dispense);
     cc_inst.change_chan(change_conn);

     SC_THREAD(reset_generator);
     sensitive<<clock;
  }
  void reset_generator()
  {
    resetn.write(0);
    wait(10);
    resetn.write(1);
  }

};

int sc_main (int argument_count, char *arguments[])
{
  top top("top");
  sc_start();
  return 0;
}

#endif

