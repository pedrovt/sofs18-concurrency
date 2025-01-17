#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "service.h"
#include "client.h"
#include "barber-shop.h"

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
static int select_requests(Client* client);
static void wait_its_turn(Client* client);
static void rise_from_client_benches(Client* client);
static void wait_all_services_done(Client* client);

static char* to_string_client(Client* client);

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
         // Pequena alteracao, so entra na loja se escolheu algum pedido
         // Visto que há percentagem para cada request, pode nao escolher nada
         if(select_requests(client) != 0) {
            wait_its_turn(client);
            rise_from_client_benches(client);
            wait_all_services_done(client);
            /* printf("Client %d went %d times\n", client->id, i+1);
            printf("Current missing trips to close: %d\n", client->shop->trips); */
         }
         i++;
      }
   }
   client->state = DONE;
   client->barberID = -1;
   log_client(client);
   notify_client_death(client);
}

static void notify_client_birth(Client* client)
{
   require (client != NULL, "client argument required");

   /** TODO:
    * 1: (if necessary) inform simulation that a new client begins its existence.
    **/
   pthread_mutex_lock(&client->shop->client_benchCR);
   client->shop->clientsOn += 1;
   pthread_mutex_unlock(&client->shop->client_benchCR);

   log_client(client);
}

static void notify_client_death(Client* client)
{
   /** TODO:
    * 1: (if necessary) inform simulation that a new client ceases its existence.
    **/

   pthread_mutex_lock(&client->shop->client_benchCR);
   client->shop->clientsOn -= 1;
   pthread_cond_broadcast(&client->shop->sit_client_benchCD);
   pthread_mutex_unlock(&client->shop->client_benchCR);

   require (client != NULL, "client argument required");

   log_client(client);
}

static void wandering_outside(Client* client)
{
   /** TODO:
    * 1: set the client state to WANDERING_OUTSIDE
    * 2. random a time interval [global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS]
    **/

   require (client != NULL, "client argument required");

   client->state = WANDERING_OUTSIDE;
   log_client(client);
   spend(random_int(global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS));

}

static int vacancy_in_barber_shop(Client* client)
{
   /** TODO:
    * 1: set the client state to WAITING_BARBERSHOP_VACANCY
    * 2: check if there is an empty seat in the client benches (at this instante, later on it may fail)
    **/

   // Zona critica, ver se existe lugares enquanto alguem se senta
   pthread_mutex_lock(&client->shop->client_benchCR); 

   require (client != NULL, "client argument required");
   client->state = WAITING_BARBERSHOP_VACANCY;
      
   int res = 0;
   if(num_available_benches_seats(client_benches(client->shop)) > 0) 
      res = 1;

   log_client(client);

   pthread_mutex_unlock(&client->shop->client_benchCR);
   
   return res;
}

static int select_requests(Client* client)
{
   /** TODO:
    * 1: set the client state to SELECTING_REQUESTS
    * 2: choose a random combination of requests
    **/

   require (client != NULL, "client argument required");

   client->state = SELECTING_REQUESTS;

   int reqs[3] = {HAIRCUT_REQ, WASH_HAIR_REQ, SHAVE_REQ};
   int probs[3] = {global->PROB_REQUEST_HAIRCUT, global->PROB_REQUEST_WASHHAIR, global->PROB_REQUEST_SHAVE};

   for(int i = 0; i < 3; i++)
      if(random_int(1, 100) <= probs[i])
         client->requests = client->requests | reqs[i];

   return client->requests;

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

   client->state = WAITING_ITS_TURN;
   log_client(client);

   client->benchesPosition = enter_barber_shop(client->shop, client->id, client->requests);
   client->barberID = greet_barber(client->shop, client->id);
   log_client(client);

}

static void rise_from_client_benches(Client* client)
{
   /** TODO:
    * 1: (exactly what the name says)
    **/

   pthread_mutex_lock(&client->shop->client_benchCR);

   require (client != NULL, "client argument required");
   require (seated_in_client_benches(client_benches(client->shop), client->id), concat_3str("client ",int2str(client->id)," not seated in benches"));

   rise_client_benches(client_benches(client->shop), client->benchesPosition, client->id);
   client->benchesPosition = -1;

   log_client(client);

   pthread_cond_signal(&client->shop->rise_client_benchCD);

   pthread_mutex_unlock(&client->shop->client_benchCR);
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
   while(client->requests != 0){
      client->state = WAITING_SERVICE;
      log_client(client);
      Service service = wait_service_from_barber(client->shop, client->id);

      client->state = WAITING_SERVICE_START;
      log_client(client);

      if(is_barber_chair_service(&service)) {
         sit_in_barber_chair(barber_chair(client->shop,service_position(&service)), client->id);
         client->state = service.request == SHAVE_REQ ? HAVING_A_SHAVE : HAVING_A_HAIRCUT;
         log_client(client);
         pthread_mutex_lock(&client->shop->waitingCR);
         while(!barber_chair_service_finished(barber_chair(client->shop,service_position(&service))))
            pthread_cond_wait(&client->shop->waitingCD, &client->shop->waitingCR);
         pthread_mutex_unlock(&client->shop->waitingCR);
         rise_from_barber_chair(barber_chair(client->shop,service_position(&service)), client->id);
      } else {
         sit_in_washbasin(washbasin(client->shop, service_position(&service)), client->id);
         client->state = HAVING_A_HAIR_WASH;
         log_client(client);
         pthread_mutex_lock(&client->shop->waitingCR);
         while(!washbasin_service_finished(washbasin(client->shop, service_position(&service))))
            pthread_cond_wait(&client->shop->waitingCD, &client->shop->waitingCR);
         pthread_mutex_unlock(&client->shop->waitingCR);
         rise_from_washbasin(washbasin(client->shop, service_position(&service)), client->id);
      }
      client->requests -= service_request(&service);
      //printf("Client %d missing requests %d\n", client->id, client->requests);
   }
   client->state = DONE;
   leave_barber_shop(client->shop, client->id);
   log_client(client);
}


static char* to_string_client(Client* client)
{
   require (client != NULL, "client argument required");

   if (client->internal == NULL)
      client->internal = (char*)mem_alloc(skel_length + 1);

   char requests[4];
   requests[0] = (client->requests & HAIRCUT_REQ) ?   'H' : ':',
   requests[1] = (client->requests & WASH_HAIR_REQ) ? 'W' : ':',
   requests[2] = (client->requests & SHAVE_REQ) ?     'S' : ':',
   requests[3] = '\0';

   char* pos = (char*)"-";
   if (client->chairPosition >= 0)
      pos = int2nstr(client->chairPosition+1, 1);
   else if (client->basinPosition >= 0)
      pos = int2nstr(client->basinPosition+1, 1);

   return gen_boxes(client->internal, skel_length, skel,
                    int2nstr(client->id, 2),
                    client->barberID > 0 ? int2nstr(client->barberID, 2) : "--",
                    requests, stateText[client->state], pos);
}

