/**
 * \brief barber shop representation
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef BARBER_SHOP_H
#define BARBER_SHOP_H

#include "global.h"
#include "barber-chair.h"
#include "tools-pot.h"
#include "washbasin.h"
#include "barber-bench.h"
#include "service.h"
#include "client-benches.h"

/* for debugging purposes only */ 
#define debug_print(id, str) \
   if (line_mode_logger())   \
   printf("%d%s\n", id, str)

#define debug_log(id, str) \
   if (line_mode_logger())   \
   send_log(id, str)

#define debug_function_run_log(id, entityid, str) \
   if (line_mode_logger())   \
   send_log(id, concat_6str("#", int2str(entityid), " [", __FUNCTION__, "] ", str))

typedef struct _BarberShop_
{
   int numBarbers;

   int numChairs;                         // num barber chairs
   BarberChair barberChair[MAX_BARBERS]; // index related with position

   int numScissors;
   int numCombs;
   int numRazors;
   ToolsPot toolsPot;

   int numWashbasins;
   Washbasin washbasin[MAX_WASHBASINS];  // index related with position

   BarberBench barberBench;

   int numClientBenchesSeats;
   int numClientBenches;
   ClientBenches clientBenches;

   int numClientsInside;
   int clientsInside[MAX_CLIENTS];

   int opened;

   int logId;
   char* internal;

   /* shared memory */
   int shmid;
   
   /* mutex semaphores */
   int mtx_shop;                          // barbershop
   int mtx_barber_benches;                // barber benches
   int mtx_clients_benches;               // client benches
   int mtx_clients_to_barbers_ids;        // clients to barbers ids
   
   /* sync semaphores */
   int sem_num_clients_in_benches;        // # clients	     in clients benches
   int sem_num_benches_pos;               // # free positions in client benches
   int sem_client_to_barber_ids;          // barbers to client ids

   /* other info */
   int barber_to_client_ids[MAX_CLIENTS]; // client ID -> barber ID 

} BarberShop;

int num_lines_barber_shop(BarberShop* shop);
int num_columns_barber_shop(BarberShop* shop);
void init_barber_shop(BarberShop* shop, int num_barbers, int num_chairs,
                      int num_scissors, int num_combs, int num_razors, int num_basins, 
                      int num_client_benches_seats, int num_client_benches);
void term_barber_shop(BarberShop* shop);
void show_barber_shop(BarberShop* shop);
void log_barber_shop(BarberShop* shop);

int valid_barber_chair_pos(BarberShop* shop, int pos);
BarberChair* barber_chair(BarberShop* shop, int pos);
ToolsPot* tools_pot(BarberShop* shop);
Washbasin* washbasin(BarberShop* shop, int pos);
BarberBench* barber_bench(BarberShop* shop);
ClientBenches* client_benches(BarberShop* shop);

int num_available_barber_chairs(BarberShop* shop);
int reserve_random_empty_barber_chair(BarberShop* shop, int barberID);
int num_available_washbasin(BarberShop* shop);
int reserve_random_empty_washbasin(BarberShop* shop, int barberID);

int is_client_inside(BarberShop* shop, int clientID);

Service wait_service_from_barber(BarberShop* shop, int barberID);
void inform_client_on_service(BarberShop* shop, Service service);

void client_done(BarberShop* shop, int clientID);

int enter_barber_shop(BarberShop* shop, int clientID, int request);
void leave_barber_shop(BarberShop* shop, int clientID);
void receive_and_greet_client(BarberShop* shop, int barberID, int clientID);
int greet_barber(BarberShop* shop, int clientID); // returns barberID

int shop_opened(BarberShop* shop);
void close_shop(BarberShop* shop); // no more outside clients accepted

/* auxiliar functions for shared memory structure (the barbershop) */
void shop_create();
void shop_destroy(BarberShop *shop);
BarberShop* shop_connect();
void shop_disconnect(BarberShop *shop);

/* auxiliar functions for semaphores */
void shop_sems_create(BarberShop *shop);
void shop_sems_destroy(BarberShop *shop);
void lock(int id);
void unlock(int id);
void down(int id);
void up(int id);
void lock(int id, int index);
void unlock(int id, int index);
void down(int id, int index);
void up(int id, int index);

/* semaphores for mutual exclusion */
int get_mtx_shop(BarberShop *shop);
int get_mtx_barber_benches(BarberShop *shop);
int get_mtx_clients_benches(BarberShop *shop);

/* semaphores for synchronization */
int get_sem_num_clients_in_benches(BarberShop *shop);
int get_sem_num_benches_pos(BarberShop *shop);

#endif
