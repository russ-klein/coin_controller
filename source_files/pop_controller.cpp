#include "coin_counter.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int random_int(int n)
{
    static bool init = false;
    if (!init) {
        init = true;
        srand((unsigned)time(NULL));
    } 
    return rand()%(n+1);
}

const static double values[] = { 0.00, 0.05, 0.10, 0.25, 0.50, 1.00 };
const static int too_long = 400;
const static int good = 0;
const static int oops = 1;

bool close(double a, double b)
{
   double diff = a - b;
   
   if (diff < 0.0) diff = diff * -1.0;
 
   if (diff < 0.005) return true;
   else return false;
}
   

int main()
{
    double purchase_price = (random_int(90) + 10) * 0.05;
    double amount_deposited = 0.0;
    double change_received = 0.0;
    double change_due;
    bool got_my_pop = false;
    bool pop;
    int wait_count;
    int random_coin;
    coin_t coin_returned;

    ac_channel<money_t>  purchase;
    ac_channel<coin_t>   coin;
    ac_channel<bool>     dispense;
    ac_channel<coin_t>   change_chan;

    Coin_counter cc;

    purchase.write(purchase_price);
    while (amount_deposited < purchase_price) {
       coin_t random_coin = random_int(4) + 1;
       coin.write( (coin_t) random_coin);
       amount_deposited += values[random_coin];
    }

    wait_count = 0;
    while (!got_my_pop) {
       cc.run(purchase, coin, dispense, change_chan);
       if (dispense.nb_read(pop)) {
          got_my_pop = true;
       }
       wait_count++;
       if (wait_count > too_long) {
          printf("Where's my pop?!? \n");
          printf("FAIL! \n");
          return oops;
       }
    }

    change_due = amount_deposited - purchase_price;
    wait_count = 0;
    while (close(change_received, change_due)) {
       cc.run(purchase, coin, dispense, change_chan);
       if (change_chan.nb_read(coin_returned)) {
          change_received += values[coin_returned.to_int()];
       }
       wait_count++;
       if (wait_count > too_long) {
          printf("Where's my change?!? \n");
          printf("expected: %15.12f got: %15.12f \n", change_due, change_received);
          printf("FAIL! \n");
          return oops;
       }
    }
    printf("PASS! \n");
    return good;
}
