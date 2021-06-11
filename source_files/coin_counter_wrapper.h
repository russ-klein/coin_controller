#ifndef COIN_COUNTER_WRAPPER_INCLUDED
#define COIN_COUNTER_WRAPPER_INCLUDED

#include "types.h"
#include "coin_counter_mc.h"

#define CONN_IN(signal, width)                     \
  sc_in< sc_lv< width > > INIT_S1(signal##_msg);   \
  sc_in<bool>             INIT_S1(signal##_val);   \
  sc_out<bool>            INIT_S1(signal##_rdy)


#define CONN_OUT(signal, width)                    \
  sc_out< sc_lv< width > >  INIT_S1(signal##_msg); \
  sc_out<bool>           INIT_S1(signal##_val);    \
  sc_in<bool>            INIT_S1(signal##_rdy)

#define WIRE_UP(instance, signal)       \
   instance.signal.msg(signal##_msg);   \
   instance.signal.rdy(signal##_rdy);   \
   instance.signal.val(signal##_val)  

// SC_MODULE(coin_counter_wrapper)
class coin_counter_wrapper : public sc_module 
{
public:
   coin_counter_mc INIT_S1(coin_counter_mc_inst);

   sc_core::sc_in<bool>   INIT_S1(clock);
   sc_core::sc_in<bool>   INIT_S1(resetn);

   money_t look_at_me = (money_t) 3.14159;
   coin_t  i_am_here  = (coin_t)  5;

   CONN_IN(purchase, WIDTH);
   CONN_IN(coin, COIN_WIDTH);
   CONN_OUT(dispense, 1);
   CONN_OUT(change, COIN_WIDTH);

   sc_clock connections_clk;
   sc_event check_event;

   SC_CTOR(coin_counter_wrapper)
        : connections_clk("connections_clk", 2, SC_NS, 0.5,0,SC_NS,true)
   {
      Connections::set_sim_clk(&connections_clk);

      SC_METHOD(check_clock);
        sensitive << connections_clk;
        sensitive << clock;

      SC_METHOD(check_event_method);
        sensitive << check_event;

      coin_counter_mc_inst.clock(clock);
      coin_counter_mc_inst.resetn(resetn);

      WIRE_UP(coin_counter_mc_inst, purchase);
      WIRE_UP(coin_counter_mc_inst, coin);
      WIRE_UP(coin_counter_mc_inst, dispense);
      WIRE_UP(coin_counter_mc_inst, change);
   }

   void check_clock() { check_event.notify(2, SC_PS);} // Let SC and Vlog delta cycles settle.

   void check_event_method() {
     if (connections_clk.read() == clock.read()) return;
     LOG("clocks misaligned!:"  << connections_clk.read() << " " << clock.read());
   }
};

#endif
