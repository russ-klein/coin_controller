
#pragma once

#include <mc_connections.h>
#include <ac_assert.h>

template <class T>
class ml_ac_channel {
public:
  Connections::Out<T>* op{0};
  Connections::In<T>* ip{0};

  void bind(Connections::Out<T>& p) {
    op = &p;
  }

  void bind(Connections::In<T>& p) {
    ip = &p;
  }

  bool available(int not_used) { 
     assert(ip != 0); 
     return true;
     return !ip->Empty(); 
  }

  T read() { 
     assert(ip != 0); 
     return(ip->Pop()); 
  }

  bool nb_read(T &value) { 
     assert(ip != 0); 
     if ( ip->Empty() ) return false; 
     value = ip->Pop(); 
     return true; 
  }

  void write(const T& v) { assert(op != 0); op->Push(v); }

  void Reset(void) {
    if (ip) ip->Reset();
    if (op) op->Reset();
  }

};

template <class T> using ac_channel = ml_ac_channel<T>;
