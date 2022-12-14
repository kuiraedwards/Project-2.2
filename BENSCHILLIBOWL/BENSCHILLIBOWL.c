
// Kuira Edwards @02942519

#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
  BENSCHILLIBOWL *bcb = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
  bcb->orders = NULL;
  bcb->current_size = 0;
  bcb->max_size = max_size;
  bcb->next_order = 1;
  bcb->handled = 0;
  bcb->expected = expected;

  pthread_mutex_init(&(bcb->mutex), NULL);
  pthread_cond_init(&(bcb->add_orders), NULL);
  pthread_cond_init(&(bcb->get_orders), NULL);

  printf("Restaurant is open!\n");
  return bcb;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  if(bcb->handled != bcb->expected) {
    fprintf(stderr, "Orders not handles!\n");
    exit(0);
  }
  pthread_mutex_destroy(&(bcb->mutex));
  free(bcb);
  printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
  pthread_mutex_lock(&(bcb->mutex));
  while(IsFull(bcb)) {
    pthread_cond_wait(&(bcb->add_orders), &(bcb->mutex));
  }
  order->order_number = bcb->next_order;
  AddOrderToBack(&(bcb->orders), order);
  bcb->next_order += 1;
  bcb->current_size += 1;
  pthread_cond_broadcast(&(bcb->get_orders));
  pthread_mutex_unlock(&(bcb->mutex));
  return order->order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
  pthread_mutex_lock(&(bcb->mutex));
  while(IsEmpty(bcb)) {
    if(bcb->handled >= bcb->expected) {
      pthread_mutex_unlock(&(bcb->mutex));
      return NULL;
    }
    pthread_cond_wait(&(bcb->get_orders), &(bcb->mutex));
  }
  Order *order = bcb->orders;
  bcb->orders = bcb->orders->next;

  bcb->current_size -= 1;
  bcb->handled += 1;

  pthread_cond_broadcast(&(bcb->add_orders));

  pthread_mutex_unlock(&(bcb->mutex));
  return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  if(bcb->current_size == 0) {
    return true;
  }
  else {
    return false;
  }
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  if(bcb->current_size == bcb->max_size) {
    return true;
  }
  else {
    return false;
  }
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
  if(*orders == NULL) {
    *orders = order;
  }
  else {
    Order *current = *orders;
    while (current->next) {
      current = current->next;
    }
    current->next = order;
  }
}