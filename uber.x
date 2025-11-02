struct Uber {
    int x;
    int y;
    int tipo;
    char placas[7];
    int status;
    int tarifa;
    double ganancia;
};

struct ViajeSolicitud {
    int x_origen;
    int y_origen;
};
struct ViajeRespuesta{
    Uber uber;
    int costo;
};
struct TerminarViajeSolicitud{
    int x_destino;
    int y_destino;
    int costoViaje;
    char placas[7];
    double distancia;
};

struct EstadoRespuesta{
    Uber uber<>;
    int cantidadUber;
    int cantidadViajes;
    int gananciaTotal;
};
program UBER_PRG{
    version UBER_VER{
        ViajeRespuesta solicitar_viaje(ViajeSolicitud) = 1;
        EstadoRespuesta solicitar_estado(void) = 2;
        void terminar_viaje(TerminarViajeSolicitud) = 3;
    } = 1;
} = 0x20000022;
