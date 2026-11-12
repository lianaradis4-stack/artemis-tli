# Simulador Artemis – Situación 3: Inyección Translunar (TLI)

UTN FRRe · Algoritmos y Estructuras de Datos · Trabajo Final Integrador (Lenguaje C)

## a) Integrantes y comisión

| Apellido y nombre | Legajo | Usuario GitHub |
|-------------------|--------|----------------|
| _completar_       |        |                |
| _completar_       |        |                |
| _completar_       |        |                |

Comisión: _completar_

## b) Escenario y reglas del juego

Desde la órbita baja terrestre (LEO), la etapa ICPS ejecuta la Inyección Translunar y deja a Orion
con velocidad `v_TLI` (oficial: 10 900 m/s, m = 26 000 kg, h = 192 km). Luego los motores se apagan
y la nave sigue una trayectoria libre hacia la Luna (D = 384 000 km).

El programa responde las 4 consignas del enunciado:

- **a)** Primera Ley de Newton: simulación de vuelo libre (inercia + gravedad que frena la nave).
- **b)** Energía cinética inicial `Ki = ½ m v²`.
- **c)** Trabajo adicional mínimo por unidad de masa de LEO al punto de equilibrio gravitatorio Tierra‑Luna, por conservación de `Em = K + U`.
- **d)** Justificación energética de por qué ir a la Luna cuesta más combustible que volver.

**Reglas del juego.** El usuario elige `h`, `v_TLI` y la masa (o usa los valores oficiales). Cada intento
se evalúa con 3 criterios (33 puntos c/u, 100 máximo):

1. La energía aportada por la TLI alcanza el punto de equilibrio.
2. La simulación llega a la vecindad de la Luna.
3. `v_TLI` < velocidad de escape (trayectoria ligada, retorno libre posible).

Se guardan hasta 10 intentos para el reporte final (opción 4, exportable a `reporte_artemis.txt`).

> Con los valores oficiales (10 900 m/s) la nave **no** llega al punto de equilibrio: el programa calcula
> una velocidad mínima de ≈ 10 905 m/s. Con `v_TLI = 11 000 m/s` la misión se cumple.

## c) Funcionamiento

```
main.c ──► interfaz.c ──► artemis.c   (física y simulación)
   │            └───────► validaciones.c
   └───────────────────► validaciones.c
```

| Módulo | Responsabilidad |
|--------|-----------------|
| `main.c` | `main()`, menú principal y ciclo de intentos. Opción oculta `--test`. |
| `artemis.h / artemis.c` | Constantes, `struct`s, cálculos analíticos, simulación velocity‑Verlet (paso 30 s, 1D Tierra‑Luna), evaluación y autotest. |
| `interfaz.h / interfaz.c` | Menú, análisis incisos a‑d, telemetría, evaluación y reporte (`FILE *`). |
| `validaciones.h / .c` | `leerEntero`, `leerDouble`, `leerSiNo`: `fgets` + `strtol/strtod`, rango, coma decimal, EOF, líneas largas. |

Se usan arreglos (`muestras[]`, `historial[]`) y punteros (parámetros `const T *`, `Intento *`, `FILE *`).
Modelo simplificado: 1D sobre la línea Tierra‑Luna, Luna fija, sin resistencia.

## d) Compilación y ejecución

```bash
make          # compila (gcc, -std=c11 -Wall -Wextra -pedantic)
./artemis     # ejecuta el simulador
make test     # autotest numérico
make clean
```

Requiere `gcc` y `make` (Linux/macOS, o MinGW/WSL en Windows). En consolas de Windows ejecutar
`chcp 65001` antes para ver bien los acentos.

## e) Registro de uso de Inteligencia Artificial

> **Completar con lo realmente hecho por el equipo** (obligatorio según el enunciado).

| Herramienta | Tarea | Prompt / consulta realizada | Cómo se validó / adaptó |
|-------------|-------|-----------------------------|--------------------------|
| Claude | Generación de la estructura base del proyecto | _pegar el prompt_ | Revisión línea por línea, compilación sin warnings, `make test`, pruebas con entradas inválidas |
| _completar_ | | | |

Todos los integrantes deben poder explicar cada línea del código en el coloquio.
