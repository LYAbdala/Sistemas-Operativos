#include "srv.h"
#include <stdio.h>
#include <stdlib.h>
/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */

void servidor(int mi_cliente)
{
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int mi_cliente_lo_tiene     = FALSE;
    int cantServers;
    MPI_Comm_size (COMM_WORLD,&cantServers); 
    cantServers = cantServers/2;
    unsigned int contador = 0;
    int buffer_enviar;
    int buffer_recibir;
    int i=0;
    int servidor=0;
    unsigned int oks[cantServers];
    unsigned int pedidos[cantServers];
    unsigned int terminaron[cantServers];
    for (i = 0; i < cantServers; ++i){oks[i]=0;pedidos[i]=0;terminaron[i]=0;}
    int todos_dan_permiso=0;
    unsigned int yo = mi_cliente-1;

    
    while( ! listo_para_salir ) {
        contador++;
        buffer_enviar=contador;
        MPI_Recv(&buffer_recibir, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        switch (tag){

        case TAG_PEDIDO:{
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;
            for(servidor=0; servidor<cantServers; servidor++){
                if(!((2*servidor) == yo)){
                    MPI_Send(&buffer_enviar, 1, MPI_INT, 2*servidor, TAG_PEDIDO_SERVERS, COMM_WORLD);
                    debug("pedi el recurso");
                }
            }
            break;
        }
        case TAG_OTORGADO_SERVERS:{
            oks[origen]=1;
            for (i = 0; i < cantServers; ++i){
                char mostrar[33];
                sprintf(mostrar,"%d",origen);
                debug(mostrar);
                todos_dan_permiso = todos_dan_permiso+oks[i];
            }
            debug("me dieron permiso");  
            if (todos_dan_permiso==2){
                debug("le doy el recurso a mi cliente");
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
                mi_cliente_lo_tiene=TRUE;
                for (i = 0; i < cantServers; ++i)oks[i]=0;
            }
            todos_dan_permiso=0;
            break;
        }

        case TAG_PEDIDO_SERVERS:{
            assert(origen != yo);
            if(!mi_cliente_lo_tiene || !hay_pedido_local){
                debug("toma");
                MPI_Send(&buffer_enviar, 1, MPI_INT, origen, TAG_OTORGADO_SERVERS, COMM_WORLD);
            }
            else{
                if(hay_pedido_local && !mi_cliente_lo_tiene){
                    unsigned int contadorDelOtro = buffer_recibir;
                    if(contadorDelOtro<contador){
                        MPI_Send(&buffer_enviar, 1, MPI_INT, origen, TAG_OTORGADO_SERVERS, COMM_WORLD);
                        debug("toma, yo no lo quiero, puto!");
                    }
                    else{
                        debug("me lo quedo, jodete");
                        pedidos[origen]=TRUE;
                    }
                }
                if(mi_cliente_lo_tiene){
                    debug("ya lo tenia yo");
                    pedidos[origen]=TRUE;
                }
            }
            break;
        }
        case TAG_LIBERO: {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (i = 0; i < cantServers; ++i)
            {
                if(2*i != yo && pedidos[i]){
                    MPI_Send(&buffer_enviar, 1, MPI_INT, 2*i, TAG_OTORGADO_SERVERS, COMM_WORLD);
                }
            }
            break;
        }
        
        case TAG_TERMINE: {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            terminaron[yo]=1;
            for (i = 0; i < cantServers; ++i)
            {
                if(2*i != yo){
                    MPI_Send(&buffer_enviar, 1, MPI_INT, 2*i, TAG_TERMINE_SERVERS, COMM_WORLD);
                }
            }
            break;
        }
        case TAG_TERMINE_SERVERS:{
            terminaron[origen]=1;
            int aux = 0;
            for (i = 0; i < cantServers; ++i)aux=aux+terminaron[i];
            if(aux==cantServers)listo_para_salir = TRUE;
            break;
        }
    }   
    }
    
}




