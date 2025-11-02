#include <stdio.h>
#include <math.h>

#include "uber.h"

#define MAX_UBERS 10
#define LONG_PLACAS 7
// DIreccion IP 192.168.100.45
typedef unsigned long ulong;
#define MAX_RAND_MOD 0x80000000UL
#define A_RAND 0x343FDUL
#define C_RAND 0x269EC3UL
#define SEED 21685489UL

static int init_done = 0;
static Uber ubers[MAX_UBERS];
static int gananciaTotal = 0;
static int viajesTotales = 0;
ulong uNuevoRand()
{
    static ulong uXn_1 = SEED;
    ulong uXn;
    uXn = (A_RAND * uXn_1 + C_RAND) % MAX_RAND_MOD;
    uXn_1 = uXn;
    return uXn;
}

void init_ubers()
{
    for (int i = 0; i < MAX_UBERS; i++)
    {
        ubers[i].x = uNuevoRand() % 51;
        ubers[i].y = uNuevoRand() % 51;
        ubers[i].tipo = uNuevoRand() % 3;
        // ubers[i].status = uNuevoRand() % 2;
        switch (ubers[i].tipo)
        {
        case 0:
            ubers[i].tarifa = 10;
            break;
        case 1:
            ubers[i].tarifa = 15;
            break;
        case 2:
            ubers[i].tarifa = 25;
            break;
        default:
            break;
        }
        ubers->ganancia = 0;
        ubers[i].status = 0;
        for (int j = 0; j < LONG_PLACAS - 1; j++)
        {
            if (j < 3)
                ubers[i].placas[j] = 'A' + (uNuevoRand() % 26);
            else
                ubers[i].placas[j] = '0' + (uNuevoRand() % 10);
        }
        ubers[i].placas[LONG_PLACAS - 1] = '\0';
    }
}

ViajeRespuesta *solicitar_viaje_1_svc(ViajeSolicitud *req, struct svc_req *svc)
{
    // Se inicializa una vez los Ubers
    if (!init_done)
    {
        init_ubers();
        init_done = 1;
    }

    double menorDistancia = MAX_RAND_MOD;
    int indiceElegido = -1;
    static ViajeRespuesta resp;

    for (int i = 0; i < MAX_UBERS; i++)
    {
        // Se escoge el uber libre y que le quede mas cerca al usuario
        if (ubers[i].status == 0)
        {
            // Se saca su norma
            double distancia = sqrt(pow(req->x_origen - ubers[i].x, 2) + pow(req->y_origen - ubers[i].y, 2));
            // Se actualiza el que este mas cerca
            if (distancia < menorDistancia)
            {
                menorDistancia = distancia;
                indiceElegido = i;
            }
        }
    }
    // Se verifca si se encontro un uber cerca y libre
    if (indiceElegido != -1)
    {
        resp.uber = ubers[indiceElegido];
        resp.uber.status = 1;
        ubers[indiceElegido].status = 1;
    }
    else
    {
        // No hay Uber libre
        resp.costo = -1;
        resp.uber.status = 1;
    }

    return &resp;
}
void *terminar_viaje_1_svc(TerminarViajeSolicitud *req, struct svc_req *svc)
{
    // Se hace un for para buscar entre los ubers
    for (int i = 0; i < MAX_UBERS; i++)
    {
        if (strcmp(ubers[i].placas, req->placas) == 0)
        {
            ubers[i].status = 0; // Se busca el uber de las mismas placas y pasa a LIBRE
            ubers[i].x = req->x_destino;
            ubers[i].y = req->y_destino;
        }
    }
    gananciaTotal += req->costoViaje; // Se aumenta la ganancia total
    viajesTotales++;                  // Se aumenta en 1 los viajes totales
}
EstadoRespuesta *solicitar_estado_1_svc(void *req, struct svc_req *svc)
{
    if (!init_done)
    {
        init_ubers();
        init_done = 1;
    }
    static EstadoRespuesta resp;
    // Se van a ver cuantos Ubers estan disponibles
    int disponibles = 0;
    for (int i = 0; i < MAX_UBERS; i++)
    {
        if (ubers[i].status == 0)
            disponibles++;
    }
    // Se crea un arreglo de ubers previamente puestos en el archivo uber.x
    resp.uber.uber_len = disponibles;                        // número real
    resp.uber.uber_val = malloc(disponibles * sizeof(Uber)); // asigna memoria

    int j = 0;
    for (int i = 0; i < MAX_UBERS; i++)
    {
        if (ubers[i].status == 0)
        {
            resp.uber.uber_val[j++] = ubers[i];
        }
    }
    resp.cantidadUber = disponibles;
    resp.cantidadViajes = viajesTotales;
    resp.gananciaTotal = gananciaTotal;

    return &resp;
}
