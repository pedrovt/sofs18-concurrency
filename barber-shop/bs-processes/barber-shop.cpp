#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "dbc.h"
#include "utils.h"
#include "box.h"
#include "logger.h"
#include "global.h"
#include "barber-shop.h"

/* TODO: take a careful look to all the non static (public) functions, to check
 * if a proper synchronization is needed.
 */


/* TODO: change here this file to your needs */

static const int skel_length = 10000;
static char skel[skel_length];

static int shmid = -1;
static Service services[MAX_BARBERS];     // TODO move to barber-shop.h

// #############################################################################
// Semaphores
/* auxiliar functions for mutual exclusion semaphores */
void unlock(int id)
{
   psem_up(id, 0);
}

void unlock(int id, int index)
{
   psem_up(id, index);
}

void lock(int id)
{
   psem_down(id, 0);
}

void lock(int id, int index)
{
   psem_down(id, index);
}

void up(int id)
{
   psem_up(id, 0);
}

void up(int id, int index)
{
   psem_up(id, index);
}

void down(int id)
{
   psem_down(id, 0);
}

void down(int id, int index)
{
   psem_down(id, index);
}

/* semaphores creation */
void shop_sems_create(BarberShop* shop)
{
   require(shop != NULL, "shop argument required");

   /* create mutex semaphores */
   shop -> mtx_shop = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
   shop -> mtx_barber_benches  = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
   shop -> mtx_clients_benches = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
   shop -> mtx_clients_to_barbers_ids = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
   
   /* create sync semaphores */
   shop -> sem_num_clients_in_benches = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
   shop -> sem_num_benches_pos	     = psemget(IPC_PRIVATE, 1, 0600 | IPC_CREAT | IPC_EXCL);
   shop -> sem_client_to_barber_ids   = psemget(IPC_PRIVATE, MAX_CLIENTS, 0600 | IPC_CREAT | IPC_EXCL);

   /* initialize mutex semaphores */
   unlock(shop -> mtx_shop);
   unlock(shop -> mtx_barber_benches);
   unlock(shop -> mtx_clients_benches);
   unlock(shop -> mtx_clients_to_barbers_ids);
   
   /* initialize sync semaphores */
   //unlock(shop -> sem_num_clients_in_benches);
   for (int i = 0; i < global ->NUM_CLIENT_BENCHES_SEATS; i++) {
      unlock(shop -> sem_num_benches_pos);
   }

   /* post-conditions for mutex semaphores */
   ensure(shop -> mtx_shop != -1, "mtx_shop semaphore not created");
   ensure(shop -> mtx_barber_benches  != -1, "mtx_barber_benches semaphore not created");
   ensure(shop -> mtx_clients_benches != -1, "mtx_clients_benches semaphore not created");

   /* post-conditions for sync semaphores */
   ensure(shop -> sem_num_clients_in_benches != -1, "sem_num_clients_in_benches semaphore not created");
   ensure(shop -> sem_num_benches_pos != -1, "sem_num_benches_pos semaphore not created");
}

/* semaphores destruction */
void shop_sems_destroy(BarberShop* shop) 
{  
   // TODO Complete
   /*psemctl(shop -> mtx_shop, 0, IPC_RMID, NULL);
   psemctl(shop -> mtx_clients_benches, 0, IPC_RMID, NULL);
   psemctl(shop -> mtx_clients_to_barbers_ids, 0)
   psemctl(shop -> sem_num_clients_in_benches, 0, IPC_RMID, NULL);
   psemctl(shop -> sem_num_benches_pos, 0, IPC_RMID, NULL);
   */
}

// #############################################################################
// Shared memory structure (the barbershop)

/* create the shared memory */
void shop_create()
{
   shmid = pshmget(IPC_PRIVATE, sizeof(BarberShop), 0600 | IPC_CREAT | IPC_EXCL);
}

/* attach shared memory to process addressing space */
BarberShop* shop_connect()
{  
   BarberShop *shop = (BarberShop*) pshmat(shmid, NULL, 0);
   ensure(shop != NULL, "shared data structure can't be null");

   return shop;
}

/* deattach shared memory to process addressing space */
void shop_disconnect(BarberShop* shop)
{
   /*
   pshmdt(shop);
   shop = NULL;
   */
}

/* ask OS to destroy the shared memory */
void shop_destroy(BarberShop* shop)
{
   /*
   pshmctl(shmid, IPC_RMID, NULL);
   shmid = -1; 
   */
}

// #############################################################################
// Getters & other util functions 
// No need to change these
static char* to_string_barber_shop(BarberShop* shop);

int num_lines_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return 1+3+num_lines_barber_chair()+num_lines_tools_pot()+3*shop->numClientBenches+1;
}

int num_columns_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   struct winsize w;
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

   return w.ws_col == 0 ? 80 : w.ws_col;
}

void init_barber_shop(BarberShop* shop, int num_barbers, int num_chairs,
                      int num_scissors, int num_combs, int num_razors, int num_basins, 
                      int num_client_benches_seats, int num_client_benches)
{
   require (shop != NULL, "shop argument required");
   require (num_barbers > 0 && num_barbers <= MAX_BARBERS, concat_5str("invalid number of barbers (", int2str(num_barbers), " not in [1,", int2str(MAX_BARBERS), "])"));
   require (num_chairs > 0 && num_chairs <= MAX_BARBER_CHAIRS, concat_5str("invalid number of chairs (", int2str(num_chairs), " not in [1,", int2str(MAX_BARBER_CHAIRS), "])"));
   require (num_scissors > 0 && num_scissors <= MAX_NUM_TOOLS, concat_5str("invalid number of scissors (", int2str(num_scissors), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_combs > 0 && num_combs <= MAX_NUM_TOOLS, concat_5str("invalid number of combs (", int2str(num_combs), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_razors > 0 && num_razors <= MAX_NUM_TOOLS, concat_5str("invalid number of razors (", int2str(num_razors), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_basins > 0 && num_basins <= MAX_WASHBASINS, concat_5str("invalid number of washbasins (", int2str(num_basins), " not in [1,", int2str(MAX_WASHBASINS), "])"));
   require (num_client_benches_seats > 0 && num_client_benches_seats <= MAX_CLIENT_BENCHES_SEATS, concat_5str("invalid number of client benches seats (", int2str(num_client_benches_seats), " not in [1,", int2str(MAX_CLIENT_BENCHES_SEATS), "])"));
   require (num_client_benches > 0 && num_client_benches <= num_client_benches_seats, concat_5str("invalid number of client benches (", int2str(num_client_benches), " not in [1,", int2str(num_client_benches_seats), "])"));

   shop->numBarbers = num_barbers;
   shop->numChairs = num_chairs;
   shop->numScissors = num_scissors;
   shop->numCombs = num_combs;
   shop->numRazors = num_razors;
   shop->numWashbasins = num_basins;
   shop->numClientBenchesSeats = num_client_benches_seats;
   shop->numClientBenches = num_client_benches;
   shop->numClientsInside = 0;
   for(int i = 0; i < MAX_CLIENTS; i++)
      shop->clientsInside[i] = 0;
   shop->opened = 1;

   gen_rect(skel, skel_length, num_lines_barber_shop(shop), num_columns_barber_shop(shop), 0xF, 1);
   gen_overlap_boxes(skel, 0, skel,
                     (char*)" BARBER SHOP ", 0, 2,
                     (char*)" Idle Barbers:", 2, 1,
                     (char*)"Barber Chairs:", 2+3, 1,
                     (char*)"Washbasins:", 3+3+num_lines_barber_chair(), num_columns_tools_pot()+3,
                     (char*)" Waiting Room:", 2+3+num_lines_barber_chair()+num_lines_tools_pot(), 1,
                     (char*)"+          +", num_lines_barber_shop(shop)-1, num_columns_barber_shop(shop)-15, NULL);

   shop->internal = (char*)mem_alloc(skel_length + 1);

   shop->logId = register_logger((char*)"Barber Shop:", 0, 0, num_lines_barber_shop(shop), num_columns_barber_shop(shop), NULL);

   // init components:
   init_barber_bench(&shop->barberBench, num_barbers, 0, 1, 16);
   for (int i = 0; i < num_chairs; i++)
      init_barber_chair(shop->barberChair+i, i+1, 1+3, 16+i*(num_columns_barber_chair()+2));
   init_tools_pot(&shop->toolsPot, num_scissors, num_combs, num_razors, 1+3+num_lines_barber_chair(), 1);
   for (int i = 0; i < num_basins; i++)
      init_washbasin(shop->washbasin+i, i+1, 1+3+num_lines_barber_chair(), num_columns_tools_pot()+3+11+1+i*(num_columns_washbasin()+2));
   init_client_benches(&shop->clientBenches, num_client_benches_seats, num_client_benches, 1+3+num_lines_barber_chair()+num_lines_tools_pot(), 16);
}

void term_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   // term components:
   term_client_benches(&shop->clientBenches);
   for (int i = 0; i < shop->numWashbasins; i++)
      term_washbasin(shop->washbasin+i);
   term_tools_pot(&shop->toolsPot);
   for (int i = 0; i < shop->numChairs; i++)
      term_barber_chair(shop->barberChair+i);
   term_barber_bench(&shop->barberBench);

   mem_free(shop->internal);
}

void show_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   log_barber_shop(shop);
   log_barber_bench(&shop->barberBench);
   for (int i = 0; i < shop->numChairs; i++)
      log_barber_chair(shop->barberChair+i);
   log_tools_pot(&shop->toolsPot);
   for (int i = 0; i < shop->numWashbasins; i++)
      log_washbasin(shop->washbasin+i);
   log_client_benches(&shop->clientBenches);
}

void log_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(shop->logId, to_string_barber_shop(shop));
}

int valid_barber_chair_pos(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");

   return pos >= 0 && pos < shop->numChairs;
}

BarberChair* barber_chair(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");
   require (valid_barber_chair_pos(shop, pos), concat_5str("invalid chair position (", int2str(pos), " not in [0,", int2str(shop->numChairs), "[)"));

   return shop->barberChair + pos;
}

ToolsPot* tools_pot(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return &shop->toolsPot;
}

int valid_washbasin_pos(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");

   return pos >= 0 && pos < shop->numWashbasins;
}

Washbasin* washbasin(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");
   require (valid_washbasin_pos(shop, pos), concat_5str("invalid basin position (", int2str(pos), " not in [0,", int2str(shop->numWashbasins), "[)"));

   return shop->washbasin + pos;
}

BarberBench* barber_bench(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return &shop->barberBench;
}

ClientBenches* client_benches(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return &shop->clientBenches;
}

int num_available_barber_chairs(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   int res = 0;
   for(int pos = 0; pos < shop->numChairs ; pos++)
      if (empty_barber_chair(shop->barberChair+pos))
         res++;

   return res;
}

int shop_opened(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");

   return shop->opened;
}

void close_shop(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");
   require(shop_opened(shop), "barber shop already closed");

   shop->opened = 0;
}

static char *to_string_barber_shop(BarberShop *shop)
{
   return gen_boxes(shop->internal, skel_length, skel);
}

// #############################################################################
// Functions to be changed/used in barber/client
int reserve_random_empty_barber_chair(BarberShop* shop, int barberID)
{
   /** TODO:
    * function called from a barber, when reserving a empty barber chair
    **/

   require (shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (num_available_barber_chairs(shop) > 0, "barber chair not available");

   int r = random_int(1, num_available_barber_chairs(shop));
   int res;
   for(res = 0; r > 0 && res < shop->numChairs ; res++)
      if (empty_barber_chair(shop->barberChair+res))
         r--;
   res--;
   reserve_barber_chair(shop->barberChair+res, barberID);

   ensure (res >= 0 && res < shop->numChairs, "");

   return res;
}

// TODO might have to be changed
int num_available_washbasin(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   int res = 0;
   for(int pos = 0; pos < shop->numWashbasins ; pos++)
      if (empty_washbasin(shop->washbasin+pos))
         res++;

   return res;
}

// TODO might have to be changed
int reserve_random_empty_washbasin(BarberShop* shop, int barberID)
{
   /** TODO:
    * function called from a barber, when reserving a empty washbasin
    **/

   require (shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (num_available_washbasin(shop) > 0, "washbasin not available");

   int r = random_int(1, num_available_washbasin(shop));
   int res;
   for(res = 0; r > 0 && res < shop->numWashbasins ; res++)
      if (empty_washbasin(shop->washbasin+res))
         r--;
   res--;
   reserve_washbasin(shop->washbasin+res, barberID);

   ensure (res >= 0 && res < shop->numWashbasins, "");

   return res;
}

// TODO might have to be changed
int is_client_inside(BarberShop* shop, int clientID)
{
   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));

   int res = 0;

   for(int i = 0; !res && i < shop->numClientsInside; i++)
      res = (clientID == shop->clientsInside[i]);

   return res;
}

// TODO not done
Service wait_service_from_barber(BarberShop* shop, int barberID)
{
   /** TODO:
    * function called from a client, expecting to be informed of the next Service to be provided by a barber
    **/

   require (shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));

   lock(shop->mtx_shop);
   while (service_used(&services[barberID - 1]))
      ;
   Service res = services[barberID - 1];
   used_service(&services[barberID - 1], 1);
   unlock(shop->mtx_shop);

   return res;
}

// TODO not done
void inform_client_on_service(BarberShop* shop, Service service)
{
   /** TODO:
    * function called from a barber, expecting to inform a client of its next service
    **/

   lock(shop->mtx_shop);
   services[service_barber_id(&service) - 1] = service;
   used_service(&services[service_barber_id(&service) - 1], 0);
   unlock(shop->mtx_shop);

   require (shop != NULL, "shop argument required");

}

// TODO not done
void client_done(BarberShop* shop, int clientID)
{
   /** TODO:
    * function called from a barber, notifying a client that all its services are done
    **/

   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   send_log(shop->logId,concat_3str("Client ",int2str(clientID), " is done"));

}

// TODO might have to be changed
int enter_barber_shop(BarberShop* shop, int clientID, int request)
{
   /** TODO:
    * Function called from a client when entering the barbershop
    **/

   //printf("\n\n\n\n\n\n\n---------------------\n\n\n\nHELLO ENTER BARBER SHOP");
   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (request > 0 && request < 8, concat_3str("invalid request (", int2str(request), ")"));
   
   require(num_available_benches_seats(client_benches(shop)) > 0, "empty seat not available in client benches");
   require (!is_client_inside(shop, clientID), concat_3str("client ", int2str(clientID), " already inside barber shop"));

   //send_log(shop->logId, concat_2str("\nBEFORE RANDOM SIT\nQUEUE SIZE IN ENTER BARBER SHOP? ", int2str(shop->clientBenches.queue.size)));
   //printf("\nRANDOM SIT");

   //log_client_benches(&shop->clientBenches);

   int res = random_sit_in_client_benches(&shop->clientBenches, clientID, request);
   
   //send_log(shop->logId, concat_2str("\nAFTER RANDOM SIT\nQUEUE SIZE IN ENTER BARBER SHOP? ", int2str(shop->clientBenches.queue.size)));

   shop -> clientsInside[shop->numClientsInside++] = clientID;
   

   return res;
}

// TODO might have to be changed
void leave_barber_shop(BarberShop* shop, int clientID)
{
   /** TODO:
    * Function called from a client when leaving the barbershop
    **/

   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (is_client_inside(shop, clientID), concat_3str("client ", int2str(clientID), " already inside barber shop"));

   int i;
   for(i = 0; shop->clientsInside[i] != clientID; i++)
      ;
   shop->numClientsInside--;
   check (shop->clientsInside[i] == clientID, "");
   for(; i < shop->numClientsInside; i++)
      shop->clientsInside[i] = shop->clientsInside[i+1];
}

// TODO
void receive_and_greet_client(BarberShop *shop, int barberID, int clientID)
{
   /** TODO:
    * function called from a barber, when receiving a new client
    * it must send the barber ID to the client
    **/
   require(shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));

   send_log(shop->logId, (char*)"[receive_and_greet_client] before lock");
   
   // !Critical area. A client can be trying to greet a barber
   lock(shop -> mtx_clients_to_barbers_ids);                                  // !LOCK
   shop -> barber_to_client_ids[clientID] = barberID;
   unlock(shop -> mtx_clients_to_barbers_ids);                                // !UNLOCK
   
   send_log(shop->logId, (char*)"[receive_and_greet_client] after lock");
   
   up(shop -> sem_client_to_barber_ids, clientID);

}

// TODO 
int greet_barber(BarberShop* shop, int clientID)
{
   /** TODO:
    * function called from a client, expecting to receive its barber's ID
    **/

   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));

   down(shop -> sem_client_to_barber_ids, clientID);
   int res = 0;
   send_log(shop->logId, (char*)"[greet_barber] before lock");
   lock(shop -> mtx_clients_to_barbers_ids);                                  // !LOCK                    
   res = shop-> barber_to_client_ids[clientID]; // should be okay, the problem is in the function above

   unlock(shop -> mtx_clients_to_barbers_ids);                                // !UNLOCK 
   send_log(shop->logId, (char*)"[greet_barber] after lock");

   send_log(shop->logId, concat_3str("[greet_barber] after gettting the id (", int2str(res), ")"));
   ensure (res > 0, concat_3str("invalid barber id (", int2str(res), ")"));

   return res;
}

// #############################################################################
// Get semaphores id
// TODO DEPRECATED. Remove them and use directly the field
// DONT REMOVE YET! 

int get_shmid_id(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");
   return shop->shmid;
}

/* semaphores for mutual exclusion */
int get_mtx_shop(BarberShop* shop)
{
   require(shop != NULL, "shop argument required");
   return shop->mtx_shop;
}

int get_mtx_barber_benches(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");
   return shop->mtx_barber_benches;
}

int get_mtx_clients_benches(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");
   return shop->mtx_clients_benches;
}

/* semaphores for sync */
int get_sem_num_clients_in_benches(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");
   return shop->sem_num_clients_in_benches;
}

int get_sem_num_benches_pos(BarberShop *shop)
{
   require(shop != NULL, "shop argument required");
   return shop->sem_num_benches_pos;
}



