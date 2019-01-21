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

extern pthread_mutex_t enterCR;
extern pthread_cond_t enterCD;
extern pthread_cond_t riseCD;

extern pthread_mutex_t greetCR;
extern pthread_cond_t greetCD;

extern pthread_mutex_t processCR;
extern pthread_mutex_t serviceCR;
extern pthread_cond_t serviceCD;
extern pthread_cond_t riseChairCD;
extern pthread_cond_t riseWashCD;
extern pthread_cond_t sitCD;

extern pthread_mutex_t toolCR;
extern pthread_cond_t scissorCD;
extern pthread_cond_t combCD;
extern pthread_cond_t razorCD;

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

   pthread_mutex_t barber_benchCR = PTHREAD_MUTEX_INITIALIZER; // Para as cadeiras dos barbeiros

   pthread_mutex_t client_benchCR = PTHREAD_MUTEX_INITIALIZER; // Para as cadeiras dos clientes 
   pthread_cond_t rise_client_benchCD = PTHREAD_COND_INITIALIZER; 
   pthread_cond_t sit_client_benchCD = PTHREAD_COND_INITIALIZER; 

   pthread_mutex_t greetCR = PTHREAD_MUTEX_INITIALIZER; // Para alterar e aceder ao mapa de greetings 
   pthread_cond_t greetCD = PTHREAD_COND_INITIALIZER; 

   pthread_mutex_t serviceCR = PTHREAD_MUTEX_INITIALIZER; // Para alterar e aceder ao mapa de servicos
   pthread_cond_t serviceCD = PTHREAD_COND_INITIALIZER; 

   pthread_mutex_t barber_chairCR = PTHREAD_MUTEX_INITIALIZER; // Para as cadeiras de barbeiro
   pthread_cond_t rise_barber_chairCD = PTHREAD_COND_INITIALIZER; 
   pthread_cond_t sit_barber_chairCD = PTHREAD_COND_INITIALIZER; 

   pthread_mutex_t washbasinCR = PTHREAD_MUTEX_INITIALIZER; // Para os lavabos
   pthread_cond_t rise_washbasinCD = PTHREAD_COND_INITIALIZER; 
   pthread_cond_t sit_washbasinCD = PTHREAD_COND_INITIALIZER;  

   pthread_mutex_t scissorCR = PTHREAD_MUTEX_INITIALIZER; // Para as tesouras
   pthread_cond_t scissor_returnCD = PTHREAD_COND_INITIALIZER;

   pthread_mutex_t combCR = PTHREAD_MUTEX_INITIALIZER; // Para os pentes
   pthread_cond_t comb_returnCD = PTHREAD_COND_INITIALIZER;

   pthread_mutex_t razorCR = PTHREAD_MUTEX_INITIALIZER; // Para os razors
   pthread_cond_t razor_returnCD = PTHREAD_COND_INITIALIZER;

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

#endif
