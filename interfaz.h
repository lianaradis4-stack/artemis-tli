/* interfaz.h - Presentación de resultados, menú y reportes por consola */
#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <stdio.h>
#include "artemis.h"

void mostrarBanner(void);
void mostrarMenu(void);
void mostrarEscenario(void);
void pedirParametros(ParamTLI *p);
void mostrarAnalisis(const ParamTLI *p, const ResTLI *r);
void mostrarSimulacion(const ParamTLI *p, const ResTLI *r, const SimTLI *s);
void mostrarEvaluacion(const Intento *in);

/* Reporte final: escribe en 'out' (stdout o un archivo) */
void imprimirReporte(FILE *out, const Intento *historial, int n);
int  guardarReporte(const char *archivo, const Intento *historial, int n);

#endif
