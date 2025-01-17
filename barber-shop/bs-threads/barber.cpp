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
static void process_resquests_from_client(Barber* barber);
static void release_client(Barber* barber);
static void done(Barber* barber);
static void process_haircut_request(Barber* barber);
static void process_hairwash_request(Barber* barber);
static void process_shave_request(Barber* barber);

static char* to_string_barber(Barber* barber);

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
      process_resquests_from_client(barber);
      release_client(barber);
      sit_in_barber_bench(barber);
      wait_for_client(barber);
   }
   done(barber);
}

static void sit_in_barber_bench(Barber* barber)
{
   /** TODO:
    * 1: sit in a random empty seat in barber bench (always available)
    **/

   // Zona critica, dois barbeiros a quererem sentar-se no mesmo banco
   
   pthread_mutex_lock(&barber->shop->barber_benchCR);

   require (barber != NULL, "barber argument required");
   require (num_seats_available_barber_bench(barber_bench(barber->shop)) > 0, "seat not available in barber shop");
   require (!seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber already seated in barber shop");

   barber->benchPosition = random_sit_in_barber_bench(barber_bench(barber->shop), barber->id);

   log_barber(barber);
   
   pthread_mutex_unlock(&barber->shop->barber_benchCR);

}

static void wait_for_client(Barber* barber)
{
   /** TODO:
    * 1: set the client state to WAITING_CLIENTS
    * 2: get next client from client benches (if empty, wait) (also, it may be required to check for simulation termination)
    * 3: receive and greet client (receive its requested services, and give back the barber's id)
    **/

   // Zona critica, dois barbeiros a aceder a fila de clientes ao mesmo tempo

   require (barber != NULL, "barber argument required");

   barber->state = WAITING_CLIENTS;
   log_barber(barber);

   int tmp = 0;
   pthread_mutex_lock(&barber->shop->client_benchCR);
   // Verificar se a lista esta vazia e mais seguro, visto que verificar se todos os lugares estao vazios
   // Depende que o cliente ja tenha chamado a funcao rise, o que causava que alguns barbeiros ficassem presos neste while
   while(empty_client_queue(&barber->shop->clientBenches.queue) && barber->shop->clientsOn != 0)
   {
      pthread_cond_wait(&barber->shop->sit_client_benchCD, &barber->shop->client_benchCR);
      //printf("Entering wait condition");
   }

   RQItem client;
   if (barber->shop->clientsOn != 0) {
      client = next_client_in_benches(client_benches(barber->shop));
      tmp = 1;
   }

   pthread_mutex_unlock(&barber->shop->client_benchCR);

   if (tmp) {
      barber->clientID = client.clientID;
      barber->reqToDo = client.request;
      log_barber(barber);  // (if necessary) more than one in proper places!!!
      receive_and_greet_client(barber->shop, barber->id, client.clientID);
   }
}

static int work_available(Barber* barber)
{
   /** TODO:
    * 1: find a safe way to solve the problem of barber termination
    **/

   require (barber != NULL, "barber argument required");

   int res = 0;
   pthread_mutex_lock(&barber->shop->client_benchCR);
   //printf("Work: %d\n", barber->shop->services);
   if (barber->shop->clientsOn != 0) res = 1;
   pthread_mutex_unlock(&barber->shop->client_benchCR);

   return res;
}

static void rise_from_barber_bench(Barber* barber)
{
   /** TODO:
    * 1: rise from the seat of barber bench
    **/

   require (barber != NULL, "barber argument required");
   require (seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber not seated in barber shop");

   rise_barber_bench(barber_bench(barber->shop), barber->benchPosition);
   barber->benchPosition = -1;

   log_barber(barber);
}

static void drop_tool(Barber * barber, const char * tool) 
{
   if (strcmp(tool, "scissor") == 0)
   {
      pthread_mutex_lock(&barber->shop->scissorCR);
      return_scissor(&barber->shop->toolsPot);
      barber->tools = barber->tools & !SCISSOR_TOOL;
      log_barber(barber);
      pthread_cond_signal(&barber->shop->scissor_returnCD);
      pthread_mutex_unlock(&barber->shop->scissorCR);
   }
   else if (strcmp(tool, "comb") == 0)
   {
      pthread_mutex_lock(&barber->shop->combCR);
      return_comb(&barber->shop->toolsPot);
      barber->tools = barber->tools & !COMB_TOOL;
      log_barber(barber);
      pthread_cond_signal(&barber->shop->comb_returnCD);
      pthread_mutex_unlock(&barber->shop->combCR);
   }
   else
   {
      pthread_mutex_lock(&barber->shop->razorCR);
      return_razor(&barber->shop->toolsPot);
      barber->tools = barber->tools & !RAZOR_TOOL;
      log_barber(barber);
      pthread_cond_signal(&barber->shop->razor_returnCD);
      pthread_mutex_unlock(&barber->shop->razorCR);
   }
}

static void drop_tools(Barber * barber, int request)
{
   if (request == HAIRCUT_REQ) 
   {
      drop_tool(barber, "scissor");
      drop_tool(barber, "comb");
   }
   else if (request == SHAVE_REQ)
   {
      drop_tool(barber, "razor");
   }
}

static void pickup_tool(Barber* barber, const char *tool)
{
   if(strcmp(tool, "scissor") == 0) {
      barber->state = REQ_SCISSOR;
      log_barber(barber);
      pthread_mutex_lock(&barber->shop->scissorCR);
      while(barber->shop->toolsPot.availScissors <= 0)
         pthread_cond_wait(&barber->shop->scissor_returnCD, &barber->shop->scissorCR);
      pick_scissor(&barber->shop->toolsPot);
      pthread_mutex_unlock(&barber->shop->scissorCR);
      barber->tools = barber->tools | SCISSOR_TOOL;
   } else if(strcmp(tool, "comb") == 0) {
      barber->state = REQ_COMB;
      log_barber(barber);
      pthread_mutex_lock(&barber->shop->combCR);
      while(barber->shop->toolsPot.availCombs <= 0)
         pthread_cond_wait(&barber->shop->comb_returnCD, &barber->shop->combCR);
      pick_comb(&barber->shop->toolsPot);
      pthread_mutex_unlock(&barber->shop->combCR);
      barber->tools = barber->tools | COMB_TOOL;
   } else {
      barber->state = REQ_RAZOR;
      log_barber(barber);
      pthread_mutex_lock(&barber->shop->razorCR);
      while(barber->shop->toolsPot.availRazors <= 0)
         pthread_cond_wait(&barber->shop->razor_returnCD, &barber->shop->razorCR);
      pick_razor(&barber->shop->toolsPot);
      pthread_mutex_unlock(&barber->shop->razorCR);
      barber->tools = barber->tools | RAZOR_TOOL;
   }
}

static void pickup_tools(Barber* barber, int request) 
{
   if(request == HAIRCUT_REQ) {
      pickup_tool(barber, "scissor");
      pickup_tool(barber, "comb");
   } else if(request == SHAVE_REQ) {
      pickup_tool(barber, "razor");
   }
}

static void process_resquests_from_client(Barber* barber)
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

   // Zona critica, varios barbeiros a chamar reserve

   require (barber != NULL, "barber argument required");

   int services[3] = {WASH_HAIR_REQ, HAIRCUT_REQ, SHAVE_REQ};
   int states[3] = {WAITING_WASHBASIN, WAITING_BARBER_SEAT, WAITING_BARBER_SEAT};
   
   // TODO ver qual e a melhor tarefa a realizar no momento, depenendo das tools existentes e dos lugares

   for(int i=0; i < 3; i++) {
      if((barber->reqToDo & services[i]) != 0) {
         Service service;
         int pos;
         barber->state = states[i];
         log_barber(barber);
         // Calcular a posicao duma cadeira random, e defenir o servico especifico em cada uma delas
         if(services[i] == HAIRCUT_REQ || services[i] == SHAVE_REQ) {
            barber->state = WAITING_BARBER_SEAT;
            log_barber(barber);
            pthread_mutex_lock(&barber->shop->barber_chairCR);
            while(num_available_barber_chairs(barber->shop) <= 0)
               pthread_cond_wait(&barber->shop->rise_barber_chairCD, &barber->shop->barber_chairCR);
            pos = reserve_random_empty_barber_chair(barber->shop, barber->id);
            pthread_mutex_unlock(&barber->shop->barber_chairCR);
            barber->chairPosition = pos;
            set_barber_chair_service(&service, barber->id, barber->clientID, pos, services[i]);
         } else {
            barber->state = WAITING_WASHBASIN;
            log_barber(barber);
            pthread_mutex_lock(&barber->shop->washbasinCR);
            while(num_available_washbasin(barber->shop) <= 0)
               pthread_cond_wait(&barber->shop->rise_washbasinCD, &barber->shop->washbasinCR);
            pos = reserve_random_empty_washbasin(barber->shop, barber->id);
            pthread_mutex_unlock(&barber->shop->washbasinCR);
            barber->basinPosition = pos;
            set_washbasin_service(&service, barber->id, barber->clientID, pos);
         }
         
         //Informar os clientes, e apanhar as ferramentas especificas
         inform_client_on_service(barber->shop, service);
         pickup_tools(barber, services[i]);

         // Se for um servico na cadeira de barbeiro defenir as ferramentas na mesma
         // Para ambos os casos espera ate o cliente estar no sitio por busy waiting
         // Wait nao e necessario pois ja nao se encontra dentro de uma zona critica
         // E o cliente tambem nao demora quase nada a sentar-se, o tempo perdido com
         // Troca de threads podia ser pior
         if(is_barber_chair_service(&service)) {
            while(!complete_barber_chair(&barber->shop->barberChair[barber->chairPosition]));       
            set_tools_barber_chair(&barber->shop->barberChair[barber->chairPosition], barber->tools);
         } else 
            while(!complete_washbasin(&barber->shop->washbasin[barber->basinPosition]));

         // Inicia o processo correspondente
         if(services[i] == HAIRCUT_REQ)
            process_haircut_request(barber);
         else if(services[i] == WASH_HAIR_REQ)
            process_hairwash_request(barber);
         else
            process_shave_request(barber);
         pthread_cond_broadcast(&barber->shop->waitingCD);

         drop_tools(barber, services[i]);

         if (is_barber_chair_service(&service))
         {
            while(barber_chair_with_a_client(barber_chair(barber->shop, barber->chairPosition)));
            release_barber_chair(barber_chair(barber->shop, barber->chairPosition), barber->id);
            pthread_cond_signal(&barber->shop->rise_barber_chairCD);
            barber->chairPosition = -1;
         }
         else
         {
            while(washbasin_with_a_client(washbasin(barber->shop, barber->basinPosition)));
            release_washbasin(washbasin(barber->shop, barber->basinPosition), barber->id);
            pthread_cond_signal(&barber->shop->rise_washbasinCD);
            barber->basinPosition = -1;
         }

         barber->reqToDo -= services[i];
         log_barber(barber);
      }
   }
}

static void release_client(Barber* barber)
{
   /** TODO:
    * 1: notify client the all the services are done
    **/

   require (barber != NULL, "barber argument required");
   /*
   if (barber->chairPosition != -1)
   {
      while(barber_chair_with_a_client(barber_chair(barber->shop, barber->chairPosition)));
      release_barber_chair(barber_chair(barber->shop, barber->chairPosition), barber->id);
      //barber->chairPosition = -1;
   }
   else
   {
      while(washbasin_with_a_client(washbasin(barber->shop, barber->basinPosition)));
      release_washbasin(washbasin(barber->shop, barber->basinPosition), barber->id);
      //barber->basinPosition = -1;
   }
   */
   log_barber(barber);
}

static void done(Barber* barber)
{
   /** TODO:
    * 1: set the client state to DONE
    **/
   require (barber != NULL, "barber argument required");

   //client_done(barber->shop, barber->clientID);
   barber->state = DONE;
   barber->clientID = -1;

   log_barber(barber);
}

static void process_haircut_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/
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
      log_barber(barber);
   }

   log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static void process_hairwash_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/
   require (barber != NULL, "barber argument required");

   barber->state = WASHING;
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
      set_completion_washbasin(washbasin(barber->shop, barber->basinPosition), complete);
      log_barber(barber);
   }

   log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static void process_shave_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/
   require (barber != NULL, "barber argument required");
   require (barber->tools & RAZOR_TOOL, "barber not holding a razor");

   barber->state = SHAVING;
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
      log_barber(barber);
   }

   log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static char* to_string_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   if (barber->internal == NULL)
      barber->internal = (char*)mem_alloc(skel_length + 1);

   char tools[4];
   tools[0] = (barber->tools & SCISSOR_TOOL) ? 'S' : '-',
      tools[1] = (barber->tools & COMB_TOOL) ?    'C' : '-',
      tools[2] = (barber->tools & RAZOR_TOOL) ?   'R' : '-',
      tools[3] = '\0';

   char* pos = (char*)"-";
   if (barber->chairPosition >= 0)
      pos = int2nstr(barber->chairPosition+1, 1);
   else if (barber->basinPosition >= 0)
      pos = int2nstr(barber->basinPosition+1, 1);

   return gen_boxes(barber->internal, skel_length, skel,
         int2nstr(barber->id, 2),
         barber->clientID > 0 ? int2nstr(barber->clientID, 2) : "--",
         tools, stateText[barber->state], pos);
}

