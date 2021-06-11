#ifndef COIN_COUNTER_MC_INCLUDED
#define COIN_COUNTER_MC_INCLUDED

#include "types.h"
#include "coin_counter.h"

SC_MODULE(coin_counter_mc)
{
  sc_in<bool> clock;
  sc_in<bool> resetn;

  Connections::In<money_t>  purchase;
  Connections::In<coin_t>   coin;
  Connections::Out<bool>    dispense;
  Connections::Out<coin_t>  change;

  ac_channel<money_t>   purchase_chan;
  ac_channel<coin_t>    coin_chan;
  ac_channel<bool>      dispense_chan;
  ac_channel<coin_t>   change_chan;

  Coin_counter coin_counter_inst;

  SC_CTOR(coin_counter_mc) {
     purchase_chan.bind(purchase);
     coin_chan.bind(coin);
     dispense_chan.bind(dispense);
     change_chan.bind(change);

     SC_THREAD(main);
     sensitive<<clock.pos();
     reset_signal_is(resetn, false);

  }
  void main() {
    purchase.Reset();
    coin.Reset();
    dispense.Reset();
    change.Reset();
    wait();
    while(1) {
      coin_counter_inst.run(
            purchase_chan,
            coin_chan,
            dispense_chan,
            change_chan
         );
      wait();
    }
  }
};

#endif
