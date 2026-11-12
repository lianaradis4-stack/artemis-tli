/* artemis.h - Situación 3: Inyección Translunar (TLI) e inercia en trayectoria libre
 * Constantes, tipos de datos y prototipos de la lógica de simulación. */
#ifndef ARTEMIS_H
#define ARTEMIS_H

/* ---------- Constantes físicas (SI) ---------- */
#define G_NEWTON          6.674e-11      /* N·m²/kg²            */
#define MASA_TIERRA       5.972e24       /* kg                  */
#define RADIO_TIERRA      6371000.0      /* m                   */
#define MASA_LUNA         7.342e22       /* kg                  */
#define RADIO_LUNA        1737400.0      /* m                   */
#define DIST_TIERRA_LUNA  384000000.0    /* m (D media)         */
#define RADIO_SOI_LUNA    66100000.0     /* m, esfera de influencia lunar */
#define VEL_ESCAPE_REF    4400.0         /* m/s, velocidad de escape de gases (valor didáctico) */

#define MU_TIERRA (G_NEWTON * MASA_TIERRA)
#define MU_LUNA   (G_NEWTON * MASA_LUNA)

/* ---------- Valores oficiales de la misión ---------- */
#define ALTURA_LEO_OFICIAL   192000.0    /* m    */
#define V_TLI_OFICIAL        10900.0     /* m/s  */
#define MASA_ORION_OFICIAL   26000.0     /* kg   */

/* ---------- Parámetros de simulación ---------- */
#define PASO_TIEMPO          30.0        /* s   (integración velocity-Verlet) */
#define TIEMPO_MAXIMO        (15.0 * 86400.0)   /* 15 días */
#define PERIODO_MUESTRA      (6.0 * 3600.0)     /* telemetría cada 6 h */
#define MAX_MUESTRAS         64
#define MAX_INTENTOS         10

/* ---------- Tipos ---------- */
typedef struct {
    double h;       /* altura de la órbita baja de partida [m]   */
    double v;       /* velocidad de inyección v_TLI [m/s]        */
    double masa;    /* masa de Orion [kg]                        */
} ParamTLI;

typedef struct {
    double r0;          /* radio orbital de partida RT + h [m]                  */
    double vCirc;       /* velocidad orbital circular en LEO [m/s]              */
    double ek;          /* energía cinética inicial [J]                         */
    double dEq;         /* distancia Tierra - punto de equilibrio gravitatorio [m] */
    double wExtKg;      /* trabajo adicional mínimo por unidad de masa [J/kg]   */
    double dEkKg;       /* energía cinética específica aportada por la TLI [J/kg] */
    double margenKg;    /* dEkKg - wExtKg  (>= 0: alcanza el equilibrio)        */
    double vMinEq;      /* v_TLI mínima para alcanzar el punto de equilibrio [m/s] */
    double vEscLeo;     /* velocidad de escape desde r0 [m/s]                   */
    double aGrav0;      /* aceleración gravitatoria terrestre en r0 [m/s²]      */
    double eEscTierra;  /* energía por kg para escapar de órbita baja terrestre [J/kg] */
    double eEscLuna;    /* energía por kg para escapar de órbita baja lunar [J/kg]     */
    double dvTierra;    /* Δv de escape desde LEO [m/s]                         */
    double dvLuna;      /* Δv de escape desde órbita lunar baja [m/s]           */
    double razonMasaTierra; /* m0/mf (Tsiolkovsky) para Δv terrestre            */
    double razonMasaLuna;   /* m0/mf (Tsiolkovsky) para Δv lunar                */
} ResTLI;

typedef struct {
    double t;       /* tiempo [s]                           */
    double x;       /* distancia al centro de la Tierra [m] */
    double v;       /* velocidad radial [m/s]               */
} MuestraTLI;

typedef enum {
    DES_LLEGA_LUNA = 0,   /* alcanza la vecindad de la Luna            */
    DES_REGRESA,          /* se frena y vuelve hacia la Tierra         */
    DES_TIEMPO            /* no concluye dentro del tiempo simulado    */
} Desenlace;

typedef struct {
    MuestraTLI muestras[MAX_MUESTRAS];
    int        n;
    int        llegoEq, llegoSOI;
    double     tEq, vEq, tSOI, tFin, xFin, vFin;
    Desenlace  desenlace;
} SimTLI;

typedef struct {
    int      nro;
    ParamTLI p;
    ResTLI   r;
    Desenlace desenlace;
    int      criterio[3];    /* 1 = cumplido */
    int      cumplidos;
    double   puntaje;        /* 0..100 */
} Intento;

/* ---------- Lógica ---------- */
void tli_calcular(const ParamTLI *p, ResTLI *r);
void tli_simular(const ParamTLI *p, const ResTLI *r, SimTLI *s);
void tli_evaluar(const ParamTLI *p, const ResTLI *r, const SimTLI *s, Intento *in);

/* Autotest numérico (devuelve cantidad de pruebas fallidas) */
int  artemis_autotest(void);

#endif
