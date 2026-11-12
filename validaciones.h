/* validaciones.h - Ingreso seguro de datos y mensajes al usuario */
#ifndef VALIDACIONES_H
#define VALIDACIONES_H

/* Leen una línea completa con fgets y la validan (tipo y rango).
 * Repiten la consulta hasta obtener un valor correcto. */
int    leerEntero(const char *mensaje, int min, int max);
double leerDouble(const char *mensaje, double min, double max);
/* Devuelve 1 para "s"/"si" y 0 para "n"/"no". */
int    leerSiNo(const char *mensaje);
void   pausar(void);

/* Mensajes de interfaz */
void   mostrarTitulo(const char *texto);
void   mostrarSeparador(void);
void   mostrarError(const char *texto);
void   mostrarAdvertencia(const char *texto);

#endif
