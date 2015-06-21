#include "srv.h"
#include <stdio.h>

#define MAX(a,b) ((a) > (b) ? a : b)

void servidor(int mi_cliente)
{
    MPI_Status status; 
    int origen;
    int tag;
    int listo_para_salir = FALSE;

    /* Constantes */
    int me;                                         /* mi id */ 
    int processesCount;                             /* cantidad total de procesos */
    MPI_Comm_size(COMM_WORLD,&processesCount);
    MPI_Comm_rank(COMM_WORLD,&me);
    int N = processesCount / 2;                     /*N:= #servidores*/

    /* Ints */
    int my_sequence_number;                         /* numero de secuencia que voy a usar en mis requests */
    int highest_sequence_number = 0;                /* numero de secuencia mas alto que vi hasta ahora */
    int remaining_replies_left;                     /* cuantas replies me faltan para poder asignar el recurso al cliente */

    /* Booleanos */
    int requesting_critical_section = FALSE;            /* indica si pedi mutex o no */
    int* reply_postponed = malloc( N * sizeof(int) );   /* indica a quienes les debo una reply atrasada */
    initializeBoolBuffer(reply_postponed,N,FALSE);
    int* isActive = malloc( N * sizeof(int) );          /* indica que servidores/clientes estan vivos */
    initializeBoolBuffer(isActive,N,TRUE);              

    /* Buffers para enviar/recibir */
    int bufferSend[2];
    int bufferRecieve[2];
    /* Este buffer esta unicamente para debugging, puede sacarse antes de la entrega */
    char debugBuffer[50];

    /* Miscelaneas */
    int i;                                              /* indice sobre arrays, en todos lados */
    int sequenceNumberFromRequest;                      /* numero de secuencia recibido de otro server */
    int serverIdFromRequest;                            /* id de server recibido de otro server */
    int deferIt;                                        /* variable auxiliar que indicara si se debe posponer la reply a otro server */

    while( ! listo_para_salir ) {

        /*Recibo mensajes de cualquier fuente*/
        MPI_Recv(&bufferRecieve,2,MPI_INT,ANY_SOURCE,ANY_TAG,COMM_WORLD,&status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        switch(tag){
            case TAG_PEDIDO:
                /* Mi cliente solicita acceso exclusivo */
                requesting_critical_section = TRUE;
                /* Genero el numero de secuencia */
                my_sequence_number = highest_sequence_number + 1;
                /* Necesito tantas replies como procesos activos - 1 */
                remaining_replies_left = countActive(isActive,N);
                remaining_replies_left--;
                /*sprintf(debugBuffer,"Me faltan tantos replies: %d",remaining_replies_left);
                debug(debugBuffer);*/
                for (i = 0; i < N; i++){
                    /* Le mando el REQUEST a todos, menos a mi mismo y a los inactivos */
                    if(!isActive[i])
                        continue;
                    int targetServer = 2*i;
                    if(targetServer == me)
                        continue;
                    bufferSend[0] = my_sequence_number;
                    bufferSend[1] = me;
                    /* Mando un REQUEST con el numero de secuencia + mi propio id */
                    MPI_Send(&bufferSend,2,MPI_INT,targetServer,TAG_REQUEST,COMM_WORLD);
                }
                break;
            case TAG_LIBERO:
                /* Mi cliente libera su acceso exclusivo*/
                requesting_critical_section = FALSE;
                /* Mando las replies pospuestas a los servers que deje colgados */
                for(i=0;i<N;i++){
                    int targetServer = 2*i;
                    if (targetServer == me)
                        continue;
                    if(!isActive[i])
                        continue;
                    if(!reply_postponed[i])
                        continue;
                    reply_postponed[i] = FALSE;
                    MPI_Send(NULL,0,MPI_INT,targetServer,TAG_REPLY,COMM_WORLD);
                }
                break;
            case TAG_TERMINE:
                /*Mi cliente avisa que terminó*/
                listo_para_salir = TRUE;
                /*Le aviso a los otros servers que termine, salvo a mi y a los inactivos*/
                for (i = 0; i < N; ++i){
                    int targetServer = 2*i;
                    if(targetServer == me)
                        continue;
                    if(!isActive[i])
                        continue;
                    MPI_Send(NULL,0,MPI_INT,targetServer,TAG_TERMINE_SERVERS,COMM_WORLD);
                }
                break;
            case TAG_REQUEST:
                /*Recibi una request de otro servidor*/
                sequenceNumberFromRequest = bufferRecieve[0];
                serverIdFromRequest = bufferRecieve[1];
                /* Actualizo el numero de secuencia mas alto, tomando el maximo entre el
                  mas alto hasta ahora y el que estoy recibiendo */
                if(sequenceNumberFromRequest > highest_sequence_number)
                    highest_sequence_number = sequenceNumberFromRequest;
                /* deferIt decide si hay que posponer o no la reply al servidor */
                if(!requesting_critical_section){
                    deferIt = FALSE;
                }
                else{
                    if(sequenceNumberFromRequest > my_sequence_number){
                        deferIt = TRUE;
                    }
                    else if(sequenceNumberFromRequest == my_sequence_number){
                        if(serverIdFromRequest > me){
                            deferIt = TRUE;
                        }
                        else{
                            deferIt = FALSE;
                        }
                    }
                    else{
                        deferIt = FALSE;
                    }
                }
                if (deferIt){
                    /* Si pospuse la reply, marco a ese server para mandarle la reply al salir de la zona critica */
                    reply_postponed[serverIdFromRequest/2] = TRUE;
                }
                else{
                    MPI_Send(NULL,0,MPI_INT,serverIdFromRequest,TAG_REPLY,COMM_WORLD);
                }
                break;
            case TAG_REPLY:
                /*Al recibir un reply, decremento los que me faltan para darle el recurso al cliente*/
                remaining_replies_left--;
                break;
            case TAG_TERMINE_SERVERS:
                /*Marco como inactivo al server que me avisa que termino*/
                isActive[origen/2] = FALSE;
                break;
            default:
                break;
        }
        if(remaining_replies_left == 0){
            /* Si ya me dieron todos el ok, aviso al cliente que tiene
             * el recurso disponible*/ 
            MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            remaining_replies_left = -1;    //lo marco con algun valor fruta para que no asigne dos veces el recurso
        }
    }
    /* Libero los buffers al terminar */
    free(reply_postponed);
    free(isActive);
}

/*Inicializa en FALSE un buffer de booleanos en value*/
void initializeBoolBuffer(int* buffer, int count, int value){
    int i;
    for (i = 0; i < count; i++)
        buffer[i] = value;
}

/* Cuenta la cantidad de booleanos de buffer en TRUE */ 
int countActive(int* buffer, int count){
    int res = 0;
    int i;
    for(i=0; i<count; i++){
        if(buffer[i])
            res++;
    }
    return res;
}