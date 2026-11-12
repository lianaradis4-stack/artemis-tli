/* artemis.c - Lógica y cálculos físicos de la Situación 3 (TLI) */
#include "artemis.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

/* Energía potencial gravitatoria específica en la línea Tierra-Luna (modelo 1D
 * colineal): potencial de la Tierra + potencial de la Luna. x = distancia al
 * centro de la Tierra. */
static double potencial(double x)
{
    return -MU_TIERRA / x - MU_LUNA / (DIST_TIERRA_LUNA - x);
}

/* Aceleración radial (hacia +x, alejándose de la Tierra) */
static double aceleracion(double x)
{
    double dl = DIST_TIERRA_LUNA - x;
    return -MU_TIERRA / (x * x) + MU_LUNA / (dl * dl);
}

/* ------------------------------------------------------------------
 * Cálculos analíticos (incisos b, c y d del enunciado)
 * ------------------------------------------------------------------ */
void tli_calcular(const ParamTLI *p, ResTLI *r)
{
    r->r0     = RADIO_TIERRA + p->h;
    r->vCirc  = sqrt(MU_TIERRA / r->r0);
    r->aGrav0 = MU_TIERRA / (r->r0 * r->r0);

    /* b) Energía cinética inicial */
    r->ek = 0.5 * p->masa * p->v * p->v;

    /* c) Punto de equilibrio gravitatorio: G·MT/x² = G·ML/(D-x)²
     *    => x = D / (1 + sqrt(ML/MT)) */
    r->dEq = DIST_TIERRA_LUNA / (1.0 + sqrt(MASA_LUNA / MASA_TIERRA));

    /* Conservación de la energía mecánica por unidad de masa:
     *   E_inicial = 1/2 vc² + U(r0)     (órbita circular en LEO)
     *   E_final   = U(d_eq)             (llega con velocidad nula)
     *   W_ext/m   = E_final - E_inicial                            */
    r->wExtKg  = potencial(r->dEq) - (0.5 * r->vCirc * r->vCirc + potencial(r->r0));
    r->dEkKg   = 0.5 * (p->v * p->v - r->vCirc * r->vCirc);   /* aportado por la TLI */
    r->margenKg = r->dEkKg - r->wExtKg;

    double v2min = r->vCirc * r->vCirc + 2.0 * r->wExtKg;
    r->vMinEq  = (v2min > 0.0) ? sqrt(v2min) : 0.0;
    r->vEscLeo = sqrt(2.0 * MU_TIERRA / r->r0);

    /* d) Comparación energética Tierra->Luna vs Luna->Tierra */
    r->eEscTierra = MU_TIERRA / (2.0 * r->r0);
    r->eEscLuna   = MU_LUNA / (2.0 * RADIO_LUNA);
    r->dvTierra   = (sqrt(2.0) - 1.0) * r->vCirc;
    r->dvLuna     = (sqrt(2.0) - 1.0) * sqrt(MU_LUNA / RADIO_LUNA);
    r->razonMasaTierra = exp(r->dvTierra / VEL_ESCAPE_REF);   /* Tsiolkovsky */
    r->razonMasaLuna   = exp(r->dvLuna   / VEL_ESCAPE_REF);
}

/* ------------------------------------------------------------------
 * Simulación por pasos de tiempo (inciso a: inercia + gravedad)
 * Integración velocity-Verlet en 1D sobre la línea Tierra-Luna con
 * propulsores apagados: solo actúa la gravedad de ambos cuerpos.
 * ------------------------------------------------------------------ */
void tli_simular(const ParamTLI *p, const ResTLI *r, SimTLI *s)
{
    double t = 0.0, x = r->r0, v = p->v, proxima = 0.0;

    memset(s, 0, sizeof *s);
    s->desenlace = DES_TIEMPO;

    while (t < TIEMPO_MAXIMO) {
        if (t >= proxima && s->n < MAX_MUESTRAS - 1) {
            s->muestras[s->n].t = t;
            s->muestras[s->n].x = x;
            s->muestras[s->n].v = v;
            s->n++;
            proxima += PERIODO_MUESTRA;
        }

        double a  = aceleracion(x);
        double xn = x + v * PASO_TIEMPO + 0.5 * a * PASO_TIEMPO * PASO_TIEMPO;

        /* Llegada a la superficie lunar (fin de la simulación) */
        if (xn >= DIST_TIERRA_LUNA - RADIO_LUNA) {
            t += PASO_TIEMPO;
            x = DIST_TIERRA_LUNA - RADIO_LUNA;
            s->desenlace = DES_LLEGA_LUNA;
            break;
        }

        double an = aceleracion(xn);
        double vn = v + 0.5 * (a + an) * PASO_TIEMPO;

        /* Eventos: cruce del punto de equilibrio y de la esfera de influencia lunar */
        if (!s->llegoEq && xn >= r->dEq) {
            s->llegoEq = 1; s->tEq = t + PASO_TIEMPO; s->vEq = vn;
        }
        if (!s->llegoSOI && xn >= DIST_TIERRA_LUNA - RADIO_SOI_LUNA) {
            s->llegoSOI = 1; s->tSOI = t + PASO_TIEMPO;
        }

        t += PASO_TIEMPO;

        /* La velocidad radial se anula antes de llegar: la nave vuelve */
        if (vn <= 0.0) {
            x = xn; v = 0.0;
            s->desenlace = DES_REGRESA;
            break;
        }
        x = xn;
        v = vn;
    }

    if (s->n < MAX_MUESTRAS) {            /* muestra final */
        s->muestras[s->n].t = t;
        s->muestras[s->n].x = x;
        s->muestras[s->n].v = v;
        s->n++;
    }
    s->tFin = t; s->xFin = x; s->vFin = v;
}

/* ------------------------------------------------------------------
 * Evaluación de la misión (3 criterios, 100 puntos)
 * ------------------------------------------------------------------ */
void tli_evaluar(const ParamTLI *p, const ResTLI *r, const SimTLI *s, Intento *in)
{
    in->p = *p;
    in->r = *r;
    in->desenlace = s->desenlace;

    in->criterio[0] = (r->margenKg >= 0.0);           /* energía suficiente hasta el equilibrio */
    in->criterio[1] = (s->desenlace == DES_LLEGA_LUNA);/* la simulación llega a la Luna          */
    in->criterio[2] = (p->v < r->vEscLeo);            /* trayectoria ligada: retorno libre posible */

    in->cumplidos = in->criterio[0] + in->criterio[1] + in->criterio[2];
    in->puntaje   = 100.0 * in->cumplidos / 3.0;
}

/* ------------------------------------------------------------------
 * Autotest
 * ------------------------------------------------------------------ */
static int cerca(const char *nombre, double obtenido, double esperado, double tolRel)
{
    int ok = fabs(obtenido - esperado) <= tolRel * fabs(esperado);
    printf("  [%s] %-34s obtenido=%.6g esperado=%.6g\n",
           ok ? "OK" : "FALLA", nombre, obtenido, esperado);
    return !ok;
}

int artemis_autotest(void)
{
    int fallas = 0;
    ParamTLI p = { ALTURA_LEO_OFICIAL, V_TLI_OFICIAL, MASA_ORION_OFICIAL };
    ResTLI r;
    SimTLI s;
    Intento in;

    printf("Autotest - Situación 3 (valores oficiales)\n");
    tli_calcular(&p, &r);
    tli_simular(&p, &r, &s);
    tli_evaluar(&p, &r, &s, &in);

    fallas += cerca("Energía cinética [J]",        r.ek,    1.544530e12, 1e-6);
    fallas += cerca("Radio orbital [m]",           r.r0,    6563000.0,   1e-9);
    fallas += cerca("Velocidad circular LEO [m/s]", r.vCirc, 7792.95,     1e-3);
    fallas += cerca("Punto de equilibrio [m]",     r.dEq,   3.4569e8,    2e-3);
    fallas += cerca("Esc. Luna / Esc. Tierra",     r.eEscTierra / r.eEscLuna, 21.5, 2e-2);

    /* Coherencia entre cálculo analítico y simulación numérica */
    int coherente = ((r.margenKg >= 0.0) == (s.llegoEq != 0)) || s.desenlace == DES_TIEMPO;
    printf("  [%s] Analítico vs simulación coherentes (margen=%.4g J/kg, llegoEq=%d)\n",
           coherente ? "OK" : "FALLA", r.margenKg, s.llegoEq);
    fallas += !coherente;

    /* Con v = v_min el margen debe ser ~0 */
    ParamTLI pm = p; pm.v = r.vMinEq;
    ResTLI rm; tli_calcular(&pm, &rm);
    int cero = fabs(rm.margenKg) < 1.0;
    printf("  [%s] Margen con v_min ~ 0 (%.3g J/kg)\n", cero ? "OK" : "FALLA", rm.margenKg);
    fallas += !cero;

    printf("Resultado: %d prueba(s) fallida(s).\n", fallas);
    return fallas;
}
