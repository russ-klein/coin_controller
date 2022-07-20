#include "types.h"


// can't put this in the class definition

static const money_t value_array[] = { 0.00, 0.05, 0.10, 0.25, 0.50, 1.00 };
static const int num_coins = 6;

class Coin_counter
{
private:
   const int max_time = 0xF0000;
   money_t purchase_price;
   money_t amount_deposited;
   ac_int<20> watchdog;

public:

   enum Coin_tokens 
   {
       slug         = 0, 
       nickel       = 1, 
       dime         = 2, 
       quarter      = 3, 
       half_dollar  = 4, 
       dollar       = 5 
   };

   Coin_counter() {
      purchase_price = 0;
      amount_deposited = 0;
   }

   void check_for_purchase(
      ac_channel<money_t> &purchase)
   {
      money_t price;

      if (purchase.nb_read(price)) {
         purchase_price = price;
         watchdog = 0;
         amount_deposited = 0;
      }
   }

   void dispense_change(
      money_t amount,
      ac_channel<coin_t> &change_chan)
   {
      int i;
      // fudge factor to account for accumulated rounding errors
      money_t fudge = value_array[1]/((money_t) 2.0); 

      for (i=num_coins-1; i>0; i--) {
         money_t n = amount + fudge;
         while (amount + fudge > value_array[i]) {
            change_chan.write(i);
            amount -= value_array[i];
         }
      }
   }

   void dispense_product(
      ac_channel<bool> &dispense,
      ac_channel<coin_t> &change_chan,
      money_t change_due)
   {
      dispense.write(true);
      dispense_change(change_due, change_chan);
      purchase_price = 0;
      amount_deposited = 0;
   }
      
   void timeout(
      ac_channel<coin_t> &change_chan)
   {
      dispense_change(amount_deposited, change_chan);
      purchase_price = 0;
      amount_deposited = 0;
   }

   void run(
      ac_channel<money_t> &purchase,
      ac_channel<coin_t> &coin,
      ac_channel<bool> &dispense,
      ac_channel<coin_t> &change_chan)
   {
      coin_t coin_type;
      money_t fudge = value_array[1]/((money_t) 2.0);
      money_t pi = 3.14159;
      bool verbose = false;

      if (purchase_price == 0) check_for_purchase(purchase);

      if (coin.nb_read(coin_type)) {
          if (verbose) printf("amount deposited: %5.2f purchase_price: %5.2f coin value: %5.2f \n", 
                                 amount_deposited.to_double(), purchase_price.to_double(), value_array[coin_type.to_int()].to_double());
          switch (coin_type.to_int()) {
             case dollar      : amount_deposited += (money_t) 1.00;  break;
             case half_dollar : amount_deposited += (money_t) 0.50;  break;
             case quarter     : amount_deposited += (money_t) 0.25;  break;
             case dime        : amount_deposited += (money_t) 0.10;  break;
             case nickel      : amount_deposited += (money_t) 0.05;  break;
             case slug        : if ((amount_deposited > 0) && (amount_deposited < 0.50)) {
                                   amount_deposited = 0.0;
                                } else {
                                   amount_deposited -= (money_t) 0.50;
                                }
                                break;
             default : printf("something went very wrong \n"); break;
          }
          watchdog = 0;
      }               

      if (purchase_price > 0) {
         watchdog++;

         if (watchdog > max_time) timeout(change_chan);
         if (amount_deposited + fudge >= purchase_price) dispense_product(dispense, change_chan, amount_deposited - purchase_price);
      }
   }
};

