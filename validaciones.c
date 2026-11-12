/* validaciones.c - Ingreso seguro de enteros, flotantes y confirmaciones */
#include "validaciones.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINEA 128

/* ---------- Mensajes ---------- */
void mostrarSeparador(void)
{
    printf("------------------------------------------------------------\n");
}

void mostrarTitulo(const char *texto)
{
    printf("\n============================================================\n");
    printf(" %s\n", texto);
    printf("============================================================\n");
}

void mostrarError(const char *texto)
{
    printf("  [ERROR] %s\n", texto);
}

void mostrarAdvertencia(const char *texto)
{
    printf("  [AVISO] %s\n", texto);
}

/* ---------- Auxiliares internos ---------- */

/* Si la entrada se cierra (Ctrl+D / Ctrl+Z) no se puede seguir preguntando:
 * se termina de forma ordenada en lugar de caer en un bucle infinito. */
static void finDeEntrada(void)
{
    printf("\n  [AVISO] Fin de la entrada de datos (EOF). Se cierra el programa.\n");
    exit(EXIT_FAILURE);
}

/* Lee una línea; si es más larga que el buffer descarta el resto y avisa. */
static void pedirLinea(const char *mensaje, char *buf, size_t tam)
{
    for (;;) {
        printf("%s", mensaje);
        fflush(stdout);
        if (fgets(buf, (int)tam, stdin) == NULL)
            finDeEntrada();

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
            return;
        }
        if (feof(stdin))            /* última línea sin salto de línea */
            return;

        int c;                      /* línea demasiado larga: vaciar stdin */
        while ((c = getchar()) != '\n' && c != EOF) { }
        mostrarAdvertencia("La entrada es demasiado larga. Intente nuevamente.");
    }
}

/* Quita espacios al inicio y al final (in situ) */
static char *recortar(char *s)
{
    while (isspace((unsigned char)*s))
        s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1]))
        s[--n] = '\0';
    return s;
}

/* Acepta la coma decimal ("1,5") además del punto */
static void normalizarComa(char *s)
{
    if (strchr(s, '.') != NULL)
        return;
    char *c = strchr(s, ',');
    if (c != NULL && strchr(c + 1, ',') == NULL)
        *c = '.';
}

/* ---------- Lectura validada ---------- */

int leerEntero(const char *mensaje, int min, int max)
{
    char buf[TAM_LINEA];
    for (;;) {
        pedirLinea(mensaje, buf, sizeof buf);
        char *s = recortar(buf);
        if (*s == '\0') {
            mostrarAdvertencia("No ingresó ningún valor.");
            continue;
        }
        char *fin;
        errno = 0;
        long v = strtol(s, &fin, 10);
        if (fin == s || *fin != '\0') {
            mostrarError("Debe ingresar un número entero (sin letras ni símbolos).");
            continue;
        }
        if (errno == ERANGE || v < min || v > max) {
            printf("  [ERROR] Valor fuera de rango. Ingrese un entero entre %d y %d.\n", min, max);
            continue;
        }
        return (int)v;
    }
}

double leerDouble(const char *mensaje, double min, double max)
{
    char buf[TAM_LINEA];
    for (;;) {
        pedirLinea(mensaje, buf, sizeof buf);
        char *s = recortar(buf);
        if (*s == '\0') {
            mostrarAdvertencia("No ingresó ningún valor.");
            continue;
        }
        normalizarComa(s);
        char *fin;
        errno = 0;
        double v = strtod(s, &fin);
        if (fin == s || *fin != '\0') {
            mostrarError("Debe ingresar un número real (ej: 10900 o 10900.5).");
            continue;
        }
        if (errno == ERANGE || !isfinite(v)) {
            mostrarError("El número es demasiado grande o no es válido.");
            continue;
        }
        if (v < min || v > max) {
            printf("  [ERROR] Valor fuera de rango. Ingrese un valor entre %.10g y %.10g.\n", min, max);
            continue;
        }
        return v;
    }
}

int leerSiNo(const char *mensaje)
{
    char buf[TAM_LINEA];
    for (;;) {
        pedirLinea(mensaje, buf, sizeof buf);
        char *s = recortar(buf);
        for (char *p = s; *p; p++)
            *p = (char)tolower((unsigned char)*p);
        if (strcmp(s, "s") == 0 || strcmp(s, "si") == 0 || strcmp(s, "sí") == 0)
            return 1;
        if (strcmp(s, "n") == 0 || strcmp(s, "no") == 0)
            return 0;
        mostrarError("Responda con 's' (sí) o 'n' (no).");
    }
}

void pausar(void)
{
    char buf[TAM_LINEA];
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    if (fgets(buf, sizeof buf, stdin) != NULL && strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}
