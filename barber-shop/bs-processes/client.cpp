#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "service.h"
#include "client.h"

enum ClientState
{
   NONE = 0,                   // initial state
   WANDERING_OUTSIDE,          // client outside barshop doing (important) things
   WAITING_BARBERSHOP_VACANCY, // a place to sit in the barber shop clients benches
   SELECTING_REQUESTS,         // choosing barber shop services
   WAITING_ITS_TURN,           // waiting for a barber assignment
   WAITING_SERVICE,            // waiting service selection (includes seat id)
   WAITING_SERVICE_START,      // client already seated in chair/basin waiting service start
   HAVING_A_HAIRCUT,           // haircut in progress
   HAVING_A_SHAVE,             // shave in progress
   HAVING_A_HAIR_WASH,         // hair wash in progress
   DONE                        // final state
};

#define State_SIZE (DONE - NONE + 1)

static const char* stateText[State_SIZE] =
{
   "---------",
   "WANDERING",
   "W VACANCY",
   "REQUESTS ",
   "Wait TURN",
   "Wt SERVCE",
   "SERV STRT",
   "HAIRCUT  ",
   "SHAVE    ",
   "HAIR WASH",
   "DONE     ",
};

static const char* skel = 
   "@---+---+---@\n"
   "|C##|B##|###|\n"
   "+---+---+-+-+\n"
   "|#########|#|\n"
   "@---------+-@";
static int skel_length = num_lines_client()*(num_columns_client()+1)*4; // extra space for (pessimistic) utf8 encoding!

static void life(Client* client);

static void notify_client_birth(Client* client);
static void notify_client_death(Client* client);
static void wandering_outside(Client* client);
static int vacancy_in_barber_shop(Client* client);
static void select_requests(Client* client);
static void wait_its_turn(Client* client);
static void rise_from_client_benches(Client* client);
static void wait_all_services_done(Client* client);

static char* to_string_client(Client* client);

// #############################################################################
// Our functions and fields

// #############################################################################
// Getters & other util functions
// No need to change these
size_t sizeof_client()
{
   return sizeof(Client);
}

int num_lines_client()
{
   return string_num_lines((char*)skel);
}

int num_columns_client()
{
   return string_num_columns((char*)skel);
}

void init_client(Client* client, int id, BarberShop* shop, int num_trips_to_barber, int line, int column)
{
   require (client != NULL, "client argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (shop != NULL, "barber shop argument required");
   require (num_trips_to_barber > 0, concat_3str("invalid number of trips to barber (", int2str(num_trips_to_barber), ")"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   client->id = id;
   client->state = NONE;
   client->shop = shop;
   client->barberID = 0;
   client->num_trips_to_barber = num_trips_to_barber;
   client->requests = 0;
   client->benchesPosition = -1;
   client->chairPosition = -1;
   client->basinPosition = -1;
   client->internal = NULL;
   client->logId = register_logger((char*)("Client:"), line ,column,
                                   num_lines_client(), num_columns_client(), NULL);
}

void term_client(Client* client)
{
   require (client != NULL, "client argument required");

   if (client->internal != NULL)
   {
      mem_free(client->internal);
      client->internal = NULL;
   }
}

void log_client(Client* client)
{
   require (client != NULL, "client argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(client->logId, to_string_client(client));
}

void* main_client(void* args)
{
   Client* client = (Client*)args;
   require (client != NULL, "client argument required");
   life(client);
   return NULL;
}

static void life(Client* client)
{
   require (client != NULL, "client argument required");

   int i = 0;
   notify_client_birth(client);
   while(i < client->num_trips_to_barber)
   {
      wandering_outside(client);
      if (vacancy_in_barber_shop(client))
      {
         select_requests(client);
         wait_its_turn(client);
         rise_from_client_benches(client);
         wait_all_services_done(client);
         i++;
      }
   }
   notify_client_death(client);
}

static char *to_string_client(Client *client)
{
   require(client != NULL, "client argument required");

   if (client->internal == NULL)
      client->internal = (char *)mem_alloc(skel_length + 1);

   char requests[4];
   requests[0] = (client->requests & HAIRCUT_REQ) ? 'H' : ':',
   requests[1] = (client->requests & WASH_HAIR_REQ) ? 'W' : ':',
   requests[2] = (client->requests & SHAVE_REQ) ? 'S' : ':',
   requests[3] = '\0';

   char *pos = (char *)"-";
   if (client->chairPosition >= 0)
      pos = int2nstr(client->chairPosition + 1, 1);
   else if (client->basinPosition >= 0)
      pos = int2nstr(client->basinPosition + 1, 1);

   return gen_boxes(client->internal, skel_length, skel,
                    int2nstr(client->id, 2),
                    client->barberID > 0 ? int2nstr(client->barberID, 2) : "--",
                    requests, stateText[client->state], pos);
}

// #############################################################################
// Functions to (maybe) be developed
static void notify_client_birth(Client* client)
{
   require (client != NULL, "client argument required");

   /** TODO:
    * 1: (if necessary) inform simulation that a new client begins its existence.
    **/
}

static void notify_client_death(Client* client)
{
   /** TODO:
    * 1: (if necessary) inform simulation that a new client ceases its existence.
    **/

   require (client != NULL, "client argument required");

   debug_function_run_log(client->logId, client->id, "Going to die");
}

// #############################################################################
// Functions to be developed

static void wandering_outside(Client* client)
{
   /** TODO:
    * 1: set the client state to WANDERING_OUTSIDE
    * 2. random a time interval [global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS]
    **/

   require (client != NULL, "client argument required");

   client -> state = WANDERING_OUTSIDE;
   spend(random_int(global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS)); // Based on line 127, barber-shop
   log_client(client);
}

static int vacancy_in_barber_shop(Client* client)
{
   /** TODO:
    * 1: set the client state to WAITING_BARBERSHOP_VACANCY
    * 2: check if there is an empty seat in the client benches (at this instante, later on it may fail)
    **/
   require (client != NULL, "client argument required");

   client -> state = WAITING_BARBERSHOP_VACANCY;
   
   int res = 0;
   if (num_available_benches_seats(client_benches(client -> shop)) > 0) { 
      res = 1;
   }

   log_client(client);

   return res;
}

static void select_requests(Client* client)
{
   /** TODO:
    * 1: set the client state to SELECTING_REQUESTS
    * 2: choose a random combination of requests
    **/

   require (client != NULL, "client argument required");

   client -> state = SELECTING_REQUESTS;

   // Requests are a value from 1 to 7 (3 bits <=> 3 services)
   int milestone1 = 0;
   
   if (milestone1) {
      client -> requests = WASH_HAIR_REQ;
   }

   else {
      if (random_int(1, 100) <= global -> PROB_REQUEST_HAIRCUT) 
      {
         client -> requests = (client -> requests) | HAIRCUT_REQ;
      }
      if (random_int(1, 100) <= global -> PROB_REQUEST_WASHHAIR)
      {
         client -> requests = (client -> requests) | WASH_HAIR_REQ;
      }
      if (random_int(1, 100) <= global -> PROB_REQUEST_SHAVE)
      {
         client -> requests = (client -> requests) | SHAVE_REQ;
      }

      if (client -> requests == 0) {
         client -> requests = WASH_HAIR_REQ;
      }
   }
   
   ensure(((client->requests) >= 1 && (client->requests) <= 7), concat_2str("invalid client request ", int2str(client->requests)));
   
   log_client(client);
}

static void wait_its_turn(Client* client)
{
   /** TODO:
    * 1: set the client state to WAITING_ITS_TURN
    * 2: enter barbershop (if necessary waiting for an empty seat)
    * 3. "handshake" with assigned barber (greet_barber)
    **/

   require (client != NULL, "client argument required");

   debug_function_run_log(client -> logId, client -> id, "");

   /* 1. set the client state */
   client -> state = WAITING_ITS_TURN;

   /* 2. enter barbershop */

   /* wait for an empty seat (down semaphore with number of positions) */
   
   /*int sem_val = psemctl(get_sem_num_free_benches_pos(client->shop), 0, GETVAL); // for debug purposes
   send_log(client->logId, concat_2str("[wait_its_turn] #free benches is: ", int2str(sem_val)));
   */
   down(client -> shop -> sem_num_free_benches_pos);

   /*
   sem_val = psemctl(get_sem_num_free_benches_pos(client->shop), 0, GETVAL); // for debug purposes
   send_log(client->logId, concat_2str("[wait_its_turn] #free benches is: ", int2str(sem_val)));
   */

   /* when the code reaches this point, we know there is
    * at least 1 free position in the client benches */
   debug_function_run_log(client -> logId, client -> id, "Free position available");

   /* get position in benches. 
    * Critical zone: 2 clients trying to access the clients bench 
    * (lock-unlock to ensure safety) */
   debug_function_run_log(client -> logId, client -> id, "Critical zone! Going to lock");
   lock(client -> shop -> mtx_clients_benches);                                  // ! LOCK
   debug_function_run_log(client -> logId, client -> id, "After lock");

   log_client_benches(&client->shop->clientBenches);
   int benchesPosition = enter_barber_shop(client->shop, client->id, client->requests);
   log_client_benches(&client->shop->clientBenches);

   debug_function_run_log(client -> logId, client -> id, concat_2str("After enter barbershop, benches position is: ", int2str(benchesPosition)));

   unlock(client -> shop -> mtx_clients_benches);                                // ! UNLOCK
   debug_function_run_log(client -> logId, client -> id, "End of critical zone! after unlock");

   /* TODO up semaphore with number of clients in benches */
   up(client -> shop -> sem_num_clients_in_benches);                             // ! UP

   client -> benchesPosition = benchesPosition;
   
   /* 3. handshake with barber */
   debug_function_run_log(client -> logId, client -> id, concat_2str("Going to greet barber with client id", int2str(client -> id)));

   client -> barberID = greet_barber(client -> shop, client -> id);
   
   debug_function_run_log(client -> logId, client -> id, concat_2str("Gretted barber. Barber ID is: ", int2str(client -> barberID)));
   
   ensure((client -> barberID) > 0, concat_3str("invalid barber id (", int2str(client->barberID), ")"));
   
   log_client(client);
}

static void rise_from_client_benches(Client* client)
{
   /** TODO:
    * 1: (exactly what the name says)
    **/
   require (client != NULL, "client argument required");
   require (seated_in_client_benches(client_benches(client->shop), client->id), concat_3str("client ",int2str(client->id)," not seated in benches"));

   debug_function_run_log(client -> logId, client -> id, "Going to rise from client bench");

   /* remove client from client benches 
    * Critical Zone: 2+ clients rising at the same time
    */
   lock(client -> shop -> mtx_clients_benches);                                  // !LOCK

   rise_client_benches(client_benches(client -> shop), client -> benchesPosition, client -> id);
   client->benchesPosition = -1;    // In valid position, ie not in the bench

   /* update # free positions in benches and number clients in benches semaphores */
   //up(client -> shop -> sem_num_free_benches_pos);
   //down(client -> shop -> sem_num_clients_in_benches);      // TODO VERIFY
   debug_function_run_log(client -> logId, client -> id, "After semaphores updates");
   
   unlock(client -> shop -> mtx_clients_benches);                                // !UNLOCK

   debug_function_run_log(client -> logId, client -> id, "Has risen from client bench");
   log_client(client);

   ensure (!seated_in_client_benches(client_benches(client->shop), client->id), concat_3str("client ",int2str(client->id)," can't be seated in benches"));

}

static void wait_all_services_done(Client* client)
{
   /** TODO:
    * Expect the realization of one request at a time, until all requests are fulfilled.
    * For each request:
    * 1: set the client state to WAITING_SERVICE
    * 2: wait_service_from_barber from barbershop
    * 3: set the client state to WAITING_SERVICE_START
    * 4: sit in proper position in destination (chair/basin depending on the service selected)
    * 5: set the client state to the active service
    * 6: rise from destination
    *
    * At the end the client must leave the barber shop
    **/

   require (client != NULL, "client argument required");
   
   // TODO for
   while(client->requests != 0){
      client -> state = WAITING_SERVICE;
      log_client(client);
      debug_function_run_log(client -> logId, client -> id, "before getting service");
      Service service = wait_service_from_barber(client -> shop, client -> barberID);
      debug_function_run_log(client -> logId, client -> id, "after getting service");
      client -> state = WAITING_SERVICE_START;
      log_client(client);

      // define destination
      if (is_washbasin_service(&service)) {     // washbasin (hair wash)
      	 debug_function_run_log(client -> logId, client -> id, "before sitting in washbasin");
         sit_in_washbasin(&(client -> shop -> washbasin[service_position(&service)]), client -> id);
         debug_function_run_log(client -> logId, client -> id, "after sitting in washbasin");
         client -> state = HAVING_A_HAIR_WASH;
         log_client(client);
         // wait to finish
         while(!washbasin_service_finished(washbasin(client->shop, service_position(&service))))
            ;
         debug_function_run_log(client -> logId, client -> id, "before getting out of washbasin");
         rise_from_washbasin(washbasin(client -> shop, service_position(&service)), client -> id);
         debug_function_run_log(client -> logId, client -> id, "after getting out of washbasin");
      }
      else if(is_barber_chair_service(&service)) {                                    // chair (haircut, chair)
         sit_in_barber_chair(&(client -> shop -> barberChair[service_position(&service)]), client->id);
         client->state = service.request == HAIRCUT_REQ ? HAVING_A_HAIRCUT : HAVING_A_SHAVE;
         log_client(client);
         // wait to finish
         while(!barber_chair_service_finished(barber_chair(client->shop, service_position(&service))))
            ;
         rise_from_barber_chair(barber_chair(client -> shop, service_position(&service)), client -> id);
      }
      debug_function_run_log(client -> logId, client -> id, "Service done");
      log_client(client);
      client->requests -= service_request(&service);
      down(client -> shop -> sem_service_completion, (client -> barberID)-1);
   }
   client -> state = DONE;
   down(client -> shop -> sem_client_leave_shop);
   debug_function_run_log(client -> logId, client -> id, "Before leaving shop");
   leave_barber_shop(client -> shop, client -> id);
   debug_function_run_log(client -> logId, client -> id, "after leaving shop");
   log_client(client); // more than one in proper places!!!

   ensure (!is_client_inside(client -> shop, client -> id), "client must leave the barber shop");
}
