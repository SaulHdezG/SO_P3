#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "uber.h"

#define MAX_UBERS 10

int main(int argc, char *argv[])
{
    srand(time(NULL));
    CLIENT *cliente;
    char *server;
    int instruction;
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s host_name instruction \n", *argv);
        exit(1);
    }

    server = argv[1];
    instruction = atoi(argv[2]);

    if ((cliente = clnt_create(server, UBER_PRG, UBER_VER, "tcp")) == (CLIENT *)NULL)
    {
        clnt_pcreateerror(server);
        exit(2);
    }

    if (instruction == 1)
    {
        // Random de origen y destino del usuario
        int x_origen = rand() % 50;
        int y_origen = rand() % 50;
        int x_destino = rand() % 50;
        int y_destino = rand() % 50;

        // Se hace la solicitud de viaje
        ViajeSolicitud viaje;
        viaje.x_origen = x_origen;
        viaje.y_origen = y_origen;
        printf("Client: Llamando a solicitar_viaje (origen: %d,%d -> destino: %d,%d)\n",
               x_origen, y_origen, x_destino, y_destino);

        // Se crea el mensaje de respuesta
        ViajeRespuesta *resp;
        // Se guarda la salida de la solicitud
        resp = solicitar_viaje_1(&viaje, cliente);

        // Si hubo un uber disponible
        if (resp->costo != -1)
        {
            int tipoUber = resp->uber.tipo;
            int tarifaUber = 0;
            char *tipoUberCadena;
            switch (tipoUber)
            {
            case 0:
                tarifaUber = 10;
                tipoUberCadena = "UberPlanet";
                break;
            case 1:
                tarifaUber = 15;
                tipoUberCadena = "UberX";
                break;
            case 2:
                tarifaUber = 25;
                tipoUberCadena = "UberBlack";
                break;
            default:
                break;
            }
            printf("Client: %s asignado en (%d,%d) con tarifa %d placas %s\n",
                   tipoUberCadena, resp->uber.x, resp->uber.y, tarifaUber, resp->uber.placas);
            printf("Viajando...\n");

            // Esta variables son para fingir los tiempos que hace el uber en llegar al usuario y lo que se hace de recorrido
            double distancia_max = 70.71;
            double distancia_min = 0.0;
            double tiempo_min = 3.0;
            double tiempo_max = 7.0;

            // Distancia que se hace el uber a la ubicacion del cliente
            double distanciaInicial = sqrt(pow(viaje.x_origen - resp->uber.x, 2) + pow(viaje.y_origen - resp->uber.y, 2));
            //  Tiempo que hace el uber a la ubicacion del cliente
            double tiempo_uber_1 = tiempo_min + ((distanciaInicial - distancia_min) / (distancia_max - distancia_min)) * (tiempo_max - tiempo_min);

            sleep((int)tiempo_uber_1);

            // Tiempo y distancia que hace el uber hacia el destino que quiere el cliente
            double distanciaFinal = sqrt(pow(viaje.x_origen - x_destino, 2) + pow(viaje.y_origen - y_destino, 2));
            // Se ve la tarifa del uber segun su tipo. Se usa un switch

            // Se arma la solicitud de viaje terminado
            TerminarViajeSolicitud viaje_terminado;

            viaje_terminado.costoViaje = (int)distanciaFinal * tarifaUber;
            strcpy(viaje_terminado.placas, resp->uber.placas);
            viaje_terminado.x_destino = x_destino;
            viaje_terminado.y_destino = y_destino;

            double tiempo_uber_2 = tiempo_min + ((distanciaFinal - distancia_min) / (distancia_max - distancia_min)) * (tiempo_max - tiempo_min);

            sleep((int)tiempo_uber_2);

            printf("Viaje terminado!, costo del viaje = %d \n", viaje_terminado.costoViaje);
            terminar_viaje_1(&viaje_terminado, cliente);
        }
        else
        {
            printf("Client: No hay Ubers disponibles.\n");
        }
        return 0;
    }
    else
{
        while (1)
        {
            EstadoRespuesta *resp = solicitar_estado_1(NULL, cliente);
            for (int i = 0; i < resp->cantidadUber; i++)
            {
                char *tipo = "";
                int caso = resp->uber.uber_val[i].tipo;
                const char *estado = (resp->uber.uber_val[i].status % 2 == 0) ? "LIBRE" : "OCUPADO";
                switch (caso)
                {
                case 0:
                    tipo = "UberPlanet";
                    break;
                case 1:
                    tipo = "UberX";
                    break;
                case 2:
                    tipo = "UberBlack";
                    break;
                default:
                    break;
                }
                printf("Uber %d: (%d,%d) Tipo %s Estado %s Placas %s\n\n",
                       i + 1, resp->uber.uber_val[i].x,
                       resp->uber.uber_val[i].y,
                       tipo,
                       estado,
                       resp->uber.uber_val[i].placas);
            }
            printf("Viajes realizados %d, Ganancia total %d\n", resp->cantidadViajes, resp->gananciaTotal);
            sleep(1);
        }
        return 0;
    }

    return 0;
}
