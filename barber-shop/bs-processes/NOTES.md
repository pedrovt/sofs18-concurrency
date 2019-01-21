# BarberShop

Complementar ao enunciado
## Introdução
* Problema de Concorrência
* Estrutura partilhada: barbearia
* Entidades: barbeiro e cliente

## Importante
* Chamar função de registo de estado para logging
* Secções críticas devem ser minimizadas (numerar as tools)
* Clientes e barbeiros identificados por ID, não pela struct
* Request dos barbeiros: valor de 1 a 7 (3 bits, 3 serviços)

## Milestones
Alcance das Milestones TÊM de estar indicada nos commits
1. 1 cliente, 1 barbeiro, 1 serviço (lavar/wash)
2. Adicionar Clientes
3. Adicionar Serviços
4. Adicionar Barbeiros

## Implementação
**Ver wrappers já existentes**

Com programação defensiva
  * EXIT_POLICY 
    * Neste projecto
  * EXECUTION_POLICY 
  * Usar asserções (`check()`) e `gdb run -l ... when` para depurar
    * Asserções sequenciais: não dependem de mais ninguém
    * Asserções concorrentes: não dependem apenas de quem está a executar a asserção 
  * Ver `set-follow-fork-mode child/parent` no `gdb`

Soluções com:
* Threads
* Processos
  * Necessário partilhar memória
    * Estrutura de dados fixa
    * Simplifica :)
  * Entidades activas <=> Processos
----
## Passos
1. Definir estruturas de dados -> Já feito :)
   * Barbershop
   * Tools Pot
   * Washbasin
2. Pontos de sincronização -> a fazer 
    * Colocar na estrutura Barbershop, client e barber
    * Colocar launch & sync end of threads/processes no módullo simulation
    * Ver ciclos de vida de cada entidade
    * Ver bem quais são as zonas críticas

3. Problemas de concorrência
    * Barbers Benches
        * Não há problemas de concorrência :)
        * 1 por barbeiro
        * Só para barbeiros
        * Há sempre pelo menos 1 livre
    * Barber Chairs
        * Há problemas de concorrência -> necessário sincronizar
        * Podemos ter mais barbeiros que cadeiras
        * Garantir que uma cadeira só é usada por um barbeiro
        * Barbeiro notifica, cliente senta-se assincronamente, barbeiro fica à espera
    * Caixa de Ferramentas/Tool Pot
        * "os garfos dos filósofos"
    * Lavatórios
        * Não requer ferramentas
        * Mesma ideia das Barber Chairs
    * Waiting Room
        * Número de lugares descorrelacionado do número de clientes
        * Clientes atendidos por ordem de chegada (field)
        * Cliente só entra se tiver lugar
          * Expectativa de lugar livre -> pergunta se pode ficar à espera até estar livre

_____
_____
## Entidades
### Cliente
* Cada processo representa n cliente (apenas para evitar demasiados processos)
* Atendido por ordem de chegada (não há posição p/sentar)

#### Lifecycle
Entra -> Espera e senta-se (busy-waiting só quando não há bancos disponíveis). Escolhe os serviços mas não a ordem -> é atendido -> atrasos dos serviços (cliente é adormecido e acordado pelo barbeiro -> NO BUSY WAITING) -> vai embora

1. Ir à barbearia
2. Escolher lugar
3. Entrar e dizer que servicos quer (sem ordem) através de 3 bits. Todos os bits a 0 não pode ser 

___________
### Barbeiro

* Barbeiros são a **única** entidade a competir por recursos.
* Recursos não são suficientes para atender todos ao mesmo tempo.
  * Não há cadeiras de corte para todos e são usadas on-demand.
  * Têm de competir pelos washbasins e tools pot.

* Barbeiro faz todos os serviços pela ordem que quer.
* Ao chamar o cliente: ele e o cliente têm de ir para o sítio certo -> sincronização e ir buscar as ferramentas.

#### Lifecycle
Bancos -> Espera por cliente -> Acordado por um cliente/barbearia fechou (FECHAR implica todos os clientes serem atendidos) -> Levantar-se -> Processar request -> Libertar cliente -> Bancos

* Cuidado com
```c
wait_for_client(barber);
while(work_available(barber))
``` 
* Pode ser uma race condition. wait_for_client tem de activar o cliente (atribui-lo a um barbeiro)
