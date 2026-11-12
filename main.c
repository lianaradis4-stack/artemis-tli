/* main.c - Menú principal y ciclo de juego (Situación 3: TLI) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "artemis.h"
#include "interfaz.h"
#include "validaciones.h"


int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--test") == 0)
        return artemis_autotest() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    Intento historial[MAX_INTENTOS];
    int n = 0;
    int opcion;

    mostrarBanner();
    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione una opción (0-4): ", 0, 4);

        switch (opcion) {
        case 1:
            mostrarEscenario();
            pausar();
            break;
        case 2:
        case 3: {
            if (n >= MAX_INTENTOS) {
                mostrarAdvertencia("Se alcanzó el máximo de intentos. Vea el reporte final.");
                break;
            }
            ParamTLI p = { ALTURA_LEO_OFICIAL, V_TLI_OFICIAL, MASA_ORION_OFICIAL };
            if (opcion == 3)
                pedirParametros(&p);
            jugarIntento(historial, &n, &p);
            break;
        }
        case 4:
            imprimirReporte(stdout, historial, n);
            if (n > 0 && leerSiNo("\n¿Guardar el reporte en 'reporte_artemis.txt'? (s/n): ")) {
                if (guardarReporte("reporte_artemis.txt", historial, n))
                    printf("  Reporte guardado correctamente.\n");
            }
            pausar();
            break;
        default:
            break;
        }
    } while (opcion != 0);

    printf("\nFin de la misión. ¡Hasta la próxima!\n");
    return EXIT_SUCCESS;
}
