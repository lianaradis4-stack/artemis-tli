/* interfaz.c - Menú, entrada de parámetros, análisis y reportes */
#include "interfaz.h"
#include "validaciones.h"

#include <math.h>
#include <stdio.h>

static const char *textoDesenlace(Desenlace d)
{
    switch (d) {
    case DES_LLEGA_LUNA: return "Llega a la vecindad de la Luna";
    case DES_REGRESA:    return "Se detiene y regresa hacia la Tierra";
    default:             return "No concluye en el tiempo simulado";
    }
}

void mostrarBanner(void)
{
    printf("\n************************************************************\n");
    printf("*   ARTEMIS - SIMULADOR DE MISIÓN                          *\n");
    printf("*   Situación 3: Inyección Translunar (TLI)                *\n");
    printf("*   UTN FRRe - Algoritmos y Estructuras de Datos           *\n");
    printf("************************************************************\n");
}

void mostrarMenu(void)
{
    mostrarTitulo("MENÚ PRINCIPAL");
    printf("  1. Ver descripción del escenario y reglas\n");
    printf("  2. Simular con valores oficiales de la misión\n");
    printf("  3. Simular con parámetros propios\n");
    printf("  4. Reporte final de desempeño\n");
    printf("  0. Salir\n");
    mostrarSeparador();
}

void mostrarEscenario(void)
{
    mostrarTitulo("ESCENARIO: INYECCIÓN TRANSLUNAR (TLI)");
    printf("Desde la órbita baja (LEO) la etapa ICPS ejecuta la TLI e impulsa\n"
           "a Orion hasta v_TLI. Luego se desacopla y los motores se apagan:\n"
           "la nave sigue una trayectoria libre hacia la Luna (D = 384.000 km).\n\n");
    printf("Valores oficiales: h = %.0f m, v_TLI = %.0f m/s, m = %.0f kg\n\n",
           ALTURA_LEO_OFICIAL, V_TLI_OFICIAL, MASA_ORION_OFICIAL);
    printf("Reglas del juego:\n"
           "  - Usted elige h, v_TLI y la masa. El programa calcula la física\n"
           "    (energía cinética, trabajo mínimo, punto de equilibrio) y simula\n"
           "    el vuelo libre por pasos de %.0f s con la gravedad Tierra+Luna.\n"
           "  - Criterios de éxito (cada intento vale hasta 100 puntos):\n"
           "      1) La energía aportada por la TLI alcanza el punto de equilibrio.\n"
           "      2) La simulación llega a la vecindad de la Luna.\n"
           "      3) v_TLI < velocidad de escape (trayectoria ligada: retorno libre).\n"
           "  - Se guardan hasta %d intentos para el reporte final.\n",
           PASO_TIEMPO, MAX_INTENTOS);
    printf("\nModelo: 1D sobre la línea Tierra-Luna, Luna fija, sin resistencia.\n");
}

/* Prompt con rango visible */
static double pedir(const char *etiqueta, const char *unidad, double min, double max)
{
    char msg[200];
    snprintf(msg, sizeof msg, "%s [%s] (entre %.10g y %.10g): ", etiqueta, unidad, min, max);
    return leerDouble(msg, min, max);
}

void pedirParametros(ParamTLI *p)
{
    mostrarTitulo("PARÁMETROS DE LA MISIÓN");
    p->h    = pedir("Altura de la órbita baja h", "km", 160.0, 2000.0) * 1000.0;
    p->v    = pedir("Velocidad de inyección v_TLI", "m/s", 5000.0, 15000.0);
    p->masa = pedir("Masa de la nave Orion", "kg", 1000.0, 200000.0);
}

void mostrarAnalisis(const ParamTLI *p, const ResTLI *r)
{
    mostrarTitulo("ANÁLISIS FÍSICO");

    printf("Datos: h = %.1f m | r0 = RT + h = %.1f m | v_TLI = %.1f m/s | m = %.1f kg\n"
           "       velocidad circular en LEO = %.2f m/s\n\n",
           p->h, r->r0, p->v, p->masa, r->vCirc);

    printf("a) Primera Ley de Newton (inercia)\n"
           "   Con los propulsores apagados, la única fuerza sobre Orion es la\n"
           "   gravedad. Por inercia tiende a conservar su velocidad (MRU), pero la\n"
           "   atracción terrestre (aceleración %.4f m/s² en r0, que decrece con 1/r²)\n"
           "   la frena progresivamente: la rapidez disminuye mientras se aleja,\n"
           "   hasta acercarse al punto de equilibrio con la Luna.\n\n", r->aGrav0);

    printf("b) Energía cinética inicial\n"
           "   Ki = 1/2 m v² = %.6e J\n\n", r->ek);

    printf("c) Trabajo adicional mínimo por unidad de masa (LEO -> equilibrio)\n"
           "   Distancia al punto de equilibrio:  d_eq = D / (1 + sqrt(ML/MT)) = %.4e m\n"
           "   Conservación Em = K + U (U = -G·MT/x - G·ML/(D-x)):\n"
           "   W_ext/m = U(d_eq) - [ 1/2 vc² + U(r0) ] = %.6e J/kg\n"
           "   Energía cinética específica aportada por la TLI = %.6e J/kg\n"
           "   Margen = %.6e J/kg  -> %s\n"
           "   Velocidad de inyección mínima para alcanzar el equilibrio: %.2f m/s\n\n",
           r->dEq, r->wExtKg, r->dEkKg, r->margenKg,
           r->margenKg >= 0.0 ? "alcanza el equilibrio" : "NO alcanza el equilibrio",
           r->vMinEq);

    printf("d) Por qué Tierra->Luna cuesta más combustible que Luna->Tierra\n"
           "   Energía por kg para escapar de una órbita baja:\n"
           "     Tierra (LEO):  %.4e J/kg   (Δv = %.0f m/s)\n"
           "     Luna (baja):   %.4e J/kg   (Δv = %.0f m/s)\n"
           "   Relación de energías: %.1f veces mayor en la Tierra.\n"
           "   El pozo de potencial terrestre es mucho más profundo (mayor masa y\n"
           "   gravedad). Como el Δv entra de forma exponencial en la ecuación de\n"
           "   Tsiolkovsky, la razón de masas m0/mf (con ve = %.0f m/s) pasa de\n"
           "   %.2f (Luna) a %.2f (Tierra): hace falta mucho más propelente.\n"
           "   (En el regreso, además, la atmósfera terrestre ayuda a frenar.)\n",
           r->eEscTierra, r->dvTierra, r->eEscLuna, r->dvLuna,
           r->eEscTierra / r->eEscLuna, VEL_ESCAPE_REF,
           r->razonMasaLuna, r->razonMasaTierra);
}

void mostrarSimulacion(const ParamTLI *p, const ResTLI *r, const SimTLI *s)
{
    (void)p;
    mostrarTitulo("SIMULACIÓN DE VUELO LIBRE (telemetría cada 6 h)");
    printf("%10s %18s %14s %14s\n", "t [h]", "distancia [km]", "v [m/s]", "a [m/s²]");
    mostrarSeparador();
    for (int i = 0; i < s->n; i++) {
        const MuestraTLI *m = &s->muestras[i];
        double dl = DIST_TIERRA_LUNA - m->x;
        double a = -MU_TIERRA / (m->x * m->x) + MU_LUNA / (dl * dl);
        printf("%10.1f %18.1f %14.2f %14.5f\n", m->t / 3600.0, m->x / 1000.0, m->v, a);
    }
    mostrarSeparador();

    if (s->llegoEq)
        printf("Cruce del punto de equilibrio (%.0f km): t = %.1f h, v = %.2f m/s\n",
               r->dEq / 1000.0, s->tEq / 3600.0, s->vEq);
    else
        printf("La nave NO alcanza el punto de equilibrio (%.0f km).\n", r->dEq / 1000.0);
    if (s->llegoSOI)
        printf("Ingreso a la esfera de influencia lunar: t = %.1f h\n", s->tSOI / 3600.0);
    printf("Fin de la simulación: t = %.1f h (%.2f días), distancia = %.0f km, v = %.2f m/s\n",
           s->tFin / 3600.0, s->tFin / 86400.0, s->xFin / 1000.0, s->vFin);
    if (s->desenlace == DES_REGRESA)
        printf("Distancia máxima alcanzada (apogeo): %.0f km\n", s->xFin / 1000.0);
    printf("Desenlace: %s\n", textoDesenlace(s->desenlace));
}

void mostrarEvaluacion(const Intento *in)
{
    static const char *txt[3] = {
        "Energía suficiente hasta el punto de equilibrio",
        "La simulación llega a la vecindad de la Luna",
        "v_TLI < velocidad de escape (trayectoria ligada)"
    };
    mostrarTitulo("EVALUACIÓN DE LA MISIÓN");
    for (int i = 0; i < 3; i++)
        printf("  [%s] %s\n", in->criterio[i] ? "OK" : "FALLA", txt[i]);
    printf("\nPuntaje del intento #%d: %.0f/100 -> %s\n", in->nro, in->puntaje,
           in->cumplidos == 3 ? "MISIÓN CUMPLIDA" : "MISIÓN NO CUMPLIDA");
}

void jugarIntento(Intento *historial, int *n, const ParamTLI *p)
{
    ResTLI r;
    SimTLI s;
    Intento *in = &historial[*n];

    tli_calcular(p, &r);          /* incisos b, c, d (cálculo analítico) */
    tli_simular(p, &r, &s);       /* inciso a (vuelo libre por pasos)    */
    in->nro = *n + 1;
    tli_evaluar(p, &r, &s, in);   /* puntaje y criterios                 */
    (*n)++;

    mostrarAnalisis(p, &r);
    pausar();
    mostrarSimulacion(p, &r, &s);
    mostrarEvaluacion(in);
    pausar();
}

void imprimirReporte(FILE *out, const Intento *h, int n)
{
    fprintf(out, "\n==================== REPORTE FINAL - TLI ====================\n");
    if (n == 0) {
        fprintf(out, "Todavía no se realizó ninguna simulación.\n");
        return;
    }
    fprintf(out, "%3s %8s %9s %9s %10s %7s  %s\n",
            "#", "h [km]", "v [m/s]", "m [kg]", "margen MJ/kg", "puntaje", "desenlace");
    int mejor = 0;
    double suma = 0.0;
    for (int i = 0; i < n; i++) {
        fprintf(out, "%3d %8.1f %9.1f %9.0f %10.3f %7.0f  %s\n",
                h[i].nro, h[i].p.h / 1000.0, h[i].p.v, h[i].p.masa,
                h[i].r.margenKg / 1e6, h[i].puntaje, textoDesenlace(h[i].desenlace));
        suma += h[i].puntaje;
        if (h[i].puntaje > h[mejor].puntaje) mejor = i;
    }
    fprintf(out, "\nIntentos: %d | Mejor: #%d (%.0f/100) | Promedio: %.1f\n",
            n, h[mejor].nro, h[mejor].puntaje, suma / n);
    fprintf(out, "Mejor intento -> Ki = %.4e J, W_ext/m = %.4e J/kg, v_min = %.1f m/s\n",
            h[mejor].r.ek, h[mejor].r.wExtKg, h[mejor].r.vMinEq);
    fprintf(out, "Calificación: %s\n",
            h[mejor].puntaje >= 99.0 ? "Comandante de misión (misión cumplida)" :
            h[mejor].puntaje >= 60.0 ? "Piloto en entrenamiento (cerca del objetivo)" :
                                       "Cadete (revise la energía de inyección)");
}

int guardarReporte(const char *archivo, const Intento *h, int n)
{
    FILE *f = fopen(archivo, "w");
    if (f == NULL) {
        mostrarError("No se pudo crear el archivo del reporte.");
        return 0;
    }
    imprimirReporte(f, h, n);
    if (fclose(f) != 0) {
        mostrarError("Error al cerrar el archivo del reporte.");
        return 0;
    }
    return 1;
}
