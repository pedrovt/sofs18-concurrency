#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "barber-shop.h"
#include "barber.h"

enum State
{
   NONE = 0,
   CUTTING,
   SHAVING,
   WASHING,
   WAITING_CLIENTS,
   WAITING_BARBER_SEAT,
   WAITING_WASHBASIN,
   REQ_SCISSOR,
   REQ_COMB,
   REQ_RAZOR,
   DONE
};

#define State_SIZE (DONE - NONE + 1)

static const char* stateText[State_SIZE] =
{
   "---------",
   "CUTTING  ",
   "SHAVING  ",
   "WASHING  ",
   "W CLIENT ", // Waiting for client
   "W SEAT   ", // Waiting for barber seat
   "W BASIN  ", // Waiting for washbasin
   "R SCISSOR", // Request a scissor
   "R COMB   ", // Request a comb
   "R RAZOR  ", // Request a razor
   "DONE     ",
};

static const char* skel = 
   "@---+---+---@\n"
   "|B##|C##|###|\n"
   "+---+---+-+-+\n"
   "|#########|#|\n"
   "@---------+-@";
static int skel_length = num_lines_barber()*(num_columns_barber()+1)*4; // extra space for (pessimistic) utf8 encoding!

static void life(Barber* barber);

static void sit_in_barber_bench(Barber* barber);
static void wait_for_client(Barber* barber);
static int work_available(Barber* barber);
static void rise_from_barber_bench(Barber* barber);
static void process_requests_from_client(Barber* barber);
static void release_client(Barber* barber);
static void done(Barber* barber);

static char* to_string_barber(Barber* barber);

// #############################################################################
// Our functions and fields
static void process_haircut_request(Barber* barber);
static void process_shave_request(Barber* barber);
static void process_hairwash_request(Barber* barber);

// #############################################################################
// Getters & other util functions
// No need to change these
size_t sizeof_barber()
{
   return sizeof(Barber);
}

int num_lines_barber()
{
   return string_num_lines((char*)skel);
}

int num_columns_barber()
{
   return string_num_columns((char*)skel);
}

void init_barber(Barber* barber, int id, BarberShop* shop, int line, int column)
{
   require (barber != NULL, "barber argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (shop != NULL, "barber shop argument required");
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   barber->id = id;
   barber->state = NONE;
   barber->shop = shop;
   barber->clientID = 0;
   barber->reqToDo = 0;
   barber->benchPosition = -1;
   barber->chairPosition = -1;
   barber->basinPosition = -1;
   barber->tools = 0;
   barber->internal = NULL;
   barber->logId = register_logger((char*)("Barber:"), line ,column,
                                   num_lines_barber(), num_columns_barber(), NULL);
}

void term_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   if (barber->internal != NULL)
   {
      mem_free(barber->internal);
      barber->internal = NULL;
   }
}

void log_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(barber->logId, to_string_barber(barber));
}

void* main_barber(void* args)
{
   Barber* barber = (Barber*)args;
   require (barber != NULL, "barber argument required");
   life(barber);
   return NULL;
}

static void life(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   sit_in_barber_bench(barber);
   wait_for_client(barber);
   while(work_available(barber)) // no more possible clients and closes barbershop
   {
      rise_from_barber_bench(barber);
      process_requests_from_client(barber);
      release_client(barber);
      sit_in_barber_bench(barber);
      wait_for_client(barber); 
   }
   done(barber);
}

static char *to_string_barber(Barber *barber)
{
   require(barber != NULL, "barber argument required");

   if (barber->internal == NULL)
      barber->internal = (char *)mem_alloc(skel_length + 1);

   char tools[4];
   tools[0] = (barber->tools & SCISSOR_TOOL) ? 'S' : '-',
   tools[1] = (barber->tools & COMB_TOOL) ? 'C' : '-',
   tools[2] = (barber->tools & RAZOR_TOOL) ? 'R' : '-',
   tools[3] = '\0';

   char *pos = (char *)"-";
   if (barber->chairPosition >= 0)
      pos = int2nstr(barber->chairPosition + 1, 1);
   else if (barber->basinPosition >= 0)
      pos = int2nstr(barber->basinPosition + 1, 1);

   return gen_boxes(barber->internal, skel_length, skel,
                    int2nstr(barber->id, 2),
                    barber->clientID > 0 ? int2nstr(barber->clientID, 2) : "--",
                    tools, stateText[barber->state], pos);
}

// #############################################################################
// Functions to be developed
static void sit_in_barber_bench(Barber* barber)
{
   /** TODO:
    * 1: sit in a random empty seat in barber bench (always available)
    **/
   lock(barber -> shop -> mtx_barber_benches);                                   //! LOCK

   require (barber != NULL, "barber argument required");
   require (num_seats_available_barber_bench(barber_bench(barber->shop)) > 0, "seat not available in barber shop");
   require (!seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber already seated in barber shop");

   /* sit in a random empty seat 
    * Critical zone: 2 barbers trying to seat in the same bench
    * (lock-unlock to ensure safety) */
   int benchPosition = random_sit_in_barber_bench(barber_bench(barber -> shop), barber -> id);
   debug_function_run_log(barber -> logId, barber -> id, concat_2str("Critical Zone! After random sit result is: ", int2str(benchPosition)));
   
   log_barber_bench(barber_bench(barber -> shop));

   unlock(barber -> shop -> mtx_barber_benches);                                 //! UNLOCK

   barber -> benchPosition = benchPosition;

   ensure ((barber -> benchPosition) >= 0, "invalid barber bench position");
   ensure (seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber must be seated in barber shop");

   debug_function_run_log(barber -> logId, barber -> id, "Barber sat down in barber benches");
}

static void wait_for_client(Barber* barber)
{
   /** TODO:
    * 1: set the client state to WAITING_CLIENTS
    * 2: get next client from client benches (if empty, wait) (also, it may be required to check for simulation termination)
    * 3: receive and greet client (receive its requested services, and give back the barber's id)
    **/

   require (barber != NULL, "barber argument required");
   debug_function_run_log(barber -> logId, barber -> id, "");
   
   /* 1. set the client state */
   barber -> state = WAITING_CLIENTS;
   log_barber(barber);

   /* check for simulation termination */
   if (barber-> shop -> opened) {
      /* Pre-2: if empty, wait 
       * down semaphore with number of clients
       */
      down(barber -> shop -> sem_num_clients_in_benches);                        // ! DOWN
      //up(barber -> shop -> sem_num_free_benches_pos);

      /* when the code reaches this point, we know there is
       * at least 1 client */

      /* 2. get next client from client benches 
       * Critical zone: 2 barbers trying to access the clients queue 
       * (lock-unlock to ensure safety) */

      debug_function_run_log(barber -> logId, barber -> id, "Critical zone! Going to lock");
      lock(barber -> shop -> mtx_clients_benches);                              // ! LOCK
      debug_function_run_log(barber -> logId, barber -> id, "Critical zone! After lock");

      log_client_benches(client_benches(barber->shop));
      
      RQItem client = next_client_in_benches(client_benches(barber->shop));
      
      barber->clientID = client.clientID;
      barber->reqToDo  = client.request;
      
      log_client_benches(client_benches(barber->shop));

      debug_function_run_log(barber -> logId, barber -> id, concat_3str("Critical zone! After next_client_in_benches, client id is: ", int2str(client.clientID), "\n"));

      unlock(barber -> shop -> mtx_clients_benches);                            // ! UNLOCK
      
      debug_function_run_log(barber -> logId, barber -> id, concat_3str("End of Critical zone! After next_client_in_benches, client id is: ", int2str(client.clientID), "\n"));

      /* 3. receive and greet client */ 
      if (client.clientID > 0)     
        receive_and_greet_client(barber->shop, barber->id, client.clientID);
      
      /* else: returns and goes to work_available function */
   }
}

static int work_available(Barber* barber)
{
   /** TODO:
    * 1: find a safe way to solve the problem of barber termination
    **/
   require (barber != NULL, "barber argument required");
   
   debug_function_run_log(barber -> logId, barber -> id, "");

   int res = 1;
   if (!shop_opened(barber -> shop)) {
      debug_function_run_log(barber -> logId, barber -> id, "Shop closed");
      
      /* down do sem?oro com # clientes nas benches */
      down(barber -> shop -> sem_num_clients_in_benches);                        //! DOWN
      
      /* critical zone */
      debug_function_run_log(barber -> logId, barber -> id, "Critical zone! Going to lock");
      lock(barber -> shop -> mtx_clients_benches);	                              //! LOCK
      
      /* remove client (with ID 0) from client benches */
      RQItem client = next_client_in_benches(client_benches(barber->shop));
      res = client.clientID;

      unlock(barber -> shop -> mtx_clients_benches);                             //! UNLOCK
      debug_function_run_log(barber -> logId, barber -> id, "End of critical zone!");

      ensure (res == 0, "Client benches MUST be empty at this point in the simulation");
   } 

   debug_function_run_log(barber -> logId, barber -> id, concat_2str("Result is ", int2str(res)));
   return res;
}

static void rise_from_barber_bench(Barber* barber){
   /** TODO:
    * 1: rise from the seat of barber bench
    **/

   lock(barber -> shop -> mtx_barber_benches);                                   //! LOCK

   require (barber != NULL, "barber argument required");
   require (seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber not seated in barber shop");

   debug_function_run_log(barber -> logId, barber -> id, "Going to rise from barber bench");
   log_barber_bench(barber_bench(barber->shop));

   rise_barber_bench(barber_bench(barber->shop), barber->benchPosition);
   barber->benchPosition = -1; // Invalid position, ie not in the bench
   
   log_barber_bench(barber_bench(barber->shop));

   unlock(barber -> shop -> mtx_barber_benches);                                 // !UNLOCK
   
   debug_function_run_log(barber -> logId, barber -> id, "Has risen from barber bench");
   log_barber(barber);
}

// #############################################################################
// Functions to be developed : requests
static void process_requests_from_client(Barber* barber)
{
   /** TODO:
    * Process one client request at a time, until all requests are fulfilled.
    * For each request:
    * 1: select the request to process (any order is acceptable)
    * 2: reserve the chair/basin for the service (setting the barber's state accordingly) 
    *    2.1: set the client state to a proper value
    *    2.2: reserve a random empty chair/basin 
    *    2.2: inform client on the service to be performed
    * 3: depending on the service, grab the necessary tools from the pot (if any)
    * 4: process the service (see [incomplete] process_haircut_request as an example)
    * 3: return the used tools to the pot (if any)
    *
    *
    * At the end the client must leave the barber shop
    **/


   require (barber != NULL, "barber argument required");

   /* Critical zones: 
    * 2+ barbers reserving material (chairs/tools/washbasins) 
    */
   int numRequests = 3;	                        
   int requests[numRequests] = {WASH_HAIR_REQ, HAIRCUT_REQ, SHAVE_REQ};  
  
   /* for each request */
   for (int i = 0; i < numRequests; i++) {
      int request = requests[i];

      /* if it's actually a request from the client */
      if ((barber->reqToDo & request) != 0){
         Service service;
         /* Wash Request -> Reserve the basin */
         if (request == WASH_HAIR_REQ) {
            barber -> state = WAITING_WASHBASIN;
            log_barber(barber); //confirm if it's the best place 

            debug_function_run_log(barber->logId, barber -> id, "before reserving washbasin");
            down(barber->shop->sem_num_washbasins);
            int basinPosition = reserve_random_empty_washbasin(barber -> shop, barber -> id);
            debug_function_run_log(barber->logId, barber -> id, "after reserving washbasin");

            barber->basinPosition = basinPosition;
            set_washbasin_service(&service, barber->id, barber->clientID, basinPosition);
         }

         /* Not Wash Request -> Reserve the barber chair */
         else {
            barber->state = WAITING_BARBER_SEAT;
            log_barber(barber); //confirm if it's the best place
            debug_function_run_log(barber->logId, barber -> id, "before reserving barber chair");
            down(barber->shop->sem_num_barber_chairs);
            int chairPosition = reserve_random_empty_barber_chair(barber->shop, barber->id);
			debug_function_run_log(barber->logId, barber -> id, "after reserving barber chair");

            barber -> chairPosition = chairPosition;
            set_barber_chair_service(&service, barber->id, barber->clientID, chairPosition, request);
         }

         /* inform client on the service to be performed */
         inform_client_on_service(barber->shop, service);
                  
         /*/ grab the necessary tools from the pot	*/
         if(request == HAIRCUT_REQ) {                     // haircut requires scissor & comb
            // pick scissor
            barber -> state = REQ_SCISSOR;
            log_barber(barber);
            debug_function_run_log(barber->logId, barber -> id, "before picking scissor");
            down(barber->shop->sem_num_items_scissors);  //requests scissors and waits until they are available
            lock(barber->shop->mtx_items_scissors);
            pick_scissor(&barber->shop->toolsPot);
            unlock(barber->shop->mtx_items_scissors);
            debug_function_run_log(barber->logId, barber -> id, "after picking scissor");
            barber->tools = barber->tools | SCISSOR_TOOL;

            // pick comb
            barber -> state = REQ_COMB;
            log_barber(barber);
            debug_function_run_log(barber->logId, barber -> id, "before picking comb");
            down(barber->shop->sem_num_items_combs);
            lock(barber->shop->mtx_items_combs);
            pick_comb(&barber->shop->toolsPot);
            unlock(barber->shop->mtx_items_combs);
            debug_function_run_log(barber->logId, barber -> id, "after picking comb");
            barber->tools = barber->tools | COMB_TOOL;
         }
         else if(request == SHAVE_REQ) {                 // shave requires a razor
            // pick razor
            barber -> state = REQ_RAZOR;
            log_barber(barber);
            debug_function_run_log(barber->logId, barber -> id, "before picking razor");
            down(barber->shop->sem_num_items_razors);
            lock(barber->shop->mtx_items_razors);
            pick_razor(&barber -> shop -> toolsPot);
            unlock(barber->shop->mtx_items_razors);
            debug_function_run_log(barber->logId, barber -> id, "after picking razor");
            barber -> tools = barber->tools | RAZOR_TOOL;
         }

         /* process requests */
         if (request == SHAVE_REQ)   {
         	down(barber -> shop -> sem_barber_requests_done, barber -> id);
          	lock(barber->shop->mtx_barber_chairs, barber -> chairPosition);
          	down(barber -> shop -> sem_ready);
         	set_tools_barber_chair(&barber->shop->barberChair[barber->chairPosition], barber->tools);
         	process_shave_request(barber);
         }
         else if (request == HAIRCUT_REQ) {
         	down(barber -> shop -> sem_barber_requests_done, barber -> id);
          	lock(barber->shop->mtx_barber_chairs, barber -> chairPosition);
          	down(barber -> shop -> sem_ready);
         	set_tools_barber_chair(&barber->shop->barberChair[barber->chairPosition], barber->tools);
         	process_haircut_request(barber);
         }
         else if (request == WASH_HAIR_REQ){
         	down(barber -> shop -> sem_barber_requests_done, barber -> id);
          	lock(barber->shop->mtx_washbasins, barber -> basinPosition);
            down(barber -> shop -> sem_ready);
          	process_hairwash_request(barber);
         }

         /* request is finished here */
         /* return the used tools to the pot (if any) */
         if (request == HAIRCUT_REQ) {
            // drop scissor
            debug_function_run_log(barber->logId, barber -> id, "dropping scissor");
            lock(barber->shop->mtx_items_scissors);
            return_scissor(&barber -> shop -> toolsPot);
            unlock(barber->shop->mtx_items_scissors);
            barber -> tools = barber -> tools & !SCISSOR_TOOL;
            up(barber->shop->sem_num_items_scissors);
            log_barber(barber);

            // drop comb
            debug_function_run_log(barber->logId, barber -> id, "dropping comb");
            lock(barber->shop->mtx_items_combs);
            return_comb(&barber -> shop -> toolsPot);
            unlock(barber->shop->mtx_items_combs);
            up(barber->shop->sem_num_items_combs);
            barber -> tools = barber -> tools & !COMB_TOOL;
            log_barber(barber);
         }
         else if (request == SHAVE_REQ){
            // drop razor
            debug_function_run_log(barber->logId, barber -> id, "dropping razor");
            lock(barber->shop->mtx_items_razors);
            return_razor(&barber -> shop -> toolsPot);
            unlock(barber->shop->mtx_items_razors);
            barber -> tools = barber -> tools & !RAZOR_TOOL;
            up(barber->shop->sem_num_items_razors);
            log_barber(barber);
         }

         /* releases the barber and the client from the chair */
         if (is_barber_chair_service(&service)){
            release_barber_chair(barber_chair(barber->shop, barber->chairPosition), barber->id);
            unlock(barber->shop->mtx_barber_chairs, barber -> chairPosition);
            up(barber->shop->sem_num_barber_chairs);
         }
         else{
            release_washbasin(washbasin(barber->shop, barber->basinPosition), barber->id);
            unlock(barber->shop->mtx_washbasins, barber -> basinPosition);
            up(barber->shop->sem_num_washbasins);
         }
         log_barber(barber);
      }
   }

   barber -> state = NONE;
   log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static void release_client(Barber* barber)
{
   /** TODO:
    * 1: notify client the all the services are done
    **/

   require (barber != NULL, "barber argument required");
   client_done(barber->shop, barber->clientID);
   barber -> clientID = 0;

   log_barber(barber);
}

static void done(Barber* barber)
{
   /** TODO:
    * 1: set the client state to DONE
    **/
   require (barber != NULL, "barber argument required");

   barber -> state = DONE;
   
   log_barber(barber);
}

// ####################################
// Functions to be developed: process requests

static void process_haircut_request(Barber* barber)
{
   require (barber != NULL, "barber argument required");
   require (barber->tools & SCISSOR_TOOL, "barber not holding a scissor");
   require (barber->tools & COMB_TOOL, "barber not holding a comb");

   barber->state = CUTTING;
   log_barber(barber);

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100)
   {
      spend(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_barber_chair(barber_chair(barber->shop, barber->chairPosition), complete);
   }
   up(barber -> shop -> sem_service_completion, (barber -> id)-1);
   up(barber -> shop -> sem_barber_requests_done, barber -> id);
   log_barber(barber);  
}

static void process_hairwash_request(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   barber->state = WASHING;
   log_barber(barber);

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100){
      spend(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_washbasin(washbasin(barber->shop, barber->basinPosition), complete);
   }
   up(barber -> shop -> sem_service_completion, (barber -> id)-1);
   up(barber -> shop -> sem_barber_requests_done, barber -> id);
   log_barber(barber);  
}

static void process_shave_request(Barber* barber)
{
   require (barber != NULL, "barber argument required");
   require (barber->tools & RAZOR_TOOL, "barber not holding a razor");

   barber->state = SHAVING;
   log_barber(barber);

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100){
      spend(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_barber_chair(barber_chair(barber->shop, barber->chairPosition), complete);
   }
   up(barber -> shop -> sem_service_completion, (barber -> id)-1);
   up(barber -> shop -> sem_barber_requests_done, barber -> id);
   log_barber(barber); 
}
