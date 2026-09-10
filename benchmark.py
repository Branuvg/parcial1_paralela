#!/usr/bin/env python3
"""
Runner de metricas (requisito individual).

Compila y ejecuta los dos programas del parcial, en orden:
    1. Suma de Riemann            (secuencial/riemann_secuencial    vs paralelo/riemann_paralelo)
    2. Histograma de temperaturas (secuencial/histograma_secuencial vs paralelo/histograma_paralelo)

Para cada uno mide speedup y eficiencia barriendo 1, 2, 4, 8, ... hilos y genera:
    docs/resultados_<nombre>.csv   datos crudos
    docs/resultados_<nombre>.md    tablas listas para pegar en el informe
    docs/img/<nombre>_metricas.png  graficas de speedup y eficiencia (si hay matplotlib)

Uso:
    python3 benchmark.py [nombre] [--reps N] [--quick]
"""
import argparse
import csv
import os
import re
import shutil
import statistics
import subprocess
import sys

RAIZ = os.path.dirname(os.path.abspath(__file__))
DOCS = os.path.join(RAIZ, "docs")
IMG = os.path.join(DOCS, "img")
TIEMPO_RE = re.compile(r"Tiempo \(s\):\s+([0-9.]+)")

PROGRAMAS = [
    ("riemann",    "secuencial/riemann_secuencial",    "paralelo/riemann_paralelo"),
    ("histograma", "secuencial/histograma_secuencial", "paralelo/histograma_paralelo"),
]


def detectar_cc():
    """En macOS 'gcc' suele ser clang (sin OpenMP); se busca un gcc real de Homebrew."""
    if sys.platform == "darwin":
        for v in ("gcc-16", "gcc-15", "gcc-14", "gcc-13"):
            if shutil.which(v):
                return v
    return None


def compilar():
    cmd = ["make", "-C", RAIZ]
    cc = detectar_cc()
    if cc:
        cmd.append(f"CC={cc}")
    print(f"$ {' '.join(cmd)}")
    subprocess.run(cmd, check=True)


def correr(rel_path, threads=None, extra=None):
    """Ejecuta un binario y devuelve el 'Tiempo (s)' que imprime."""
    env = os.environ.copy()
    if threads is not None:
        env["OMP_NUM_THREADS"] = str(threads)
    cmd = [os.path.join(RAIZ, rel_path)] + (extra or [])
    out = subprocess.run(cmd, capture_output=True, text=True, env=env, check=True).stdout
    m = TIEMPO_RE.search(out)
    if not m:
        sys.exit(f"no se encontro 'Tiempo (s)' en la salida de {rel_path}:\n{out}")
    return float(m.group(1))


def mediana_de(rel_path, reps, **kw):
    return statistics.median(correr(rel_path, **kw) for _ in range(reps))


def lista_hilos(maxh):
    hs, h = [], 1
    while h < maxh:
        hs.append(h)
        h *= 2
    hs.append(maxh)
    return hs


def verificar_histograma():
    print("\n# Verificacion de correctitud del histograma paralelo")
    r = subprocess.run(
        [os.path.join(RAIZ, "paralelo/histograma_paralelo"), "--verificar"],
        capture_output=True, text=True,
    )
    print(r.stdout.strip())
    if r.returncode != 0:
        sys.exit("El histograma paralelo NO coincide con el secuencial. "
                 "Compilar con GCC real (make CC=gcc-16 en macOS).")


def bench_programa(nombre, sec, par, hilos, reps):
    """Devuelve (t_seq, [{'hilos','t','speedup','eficiencia'}, ...])."""
    print(f"\n{'='*62}\n {nombre.upper()}\n{'='*62}")
    t_seq = mediana_de(sec, reps)
    print(f"Secuencial: {t_seq:.6f} s  (mediana de {reps})\n")
    print(f"{'Hilos':>6} {'T (s)':>12} {'Speedup':>9} {'Eficiencia':>12}")
    print("-" * 43)

    puntos = []
    for p in hilos:
        t = mediana_de(par, reps, threads=p)
        sp = t_seq / t
        ef = sp / p
        puntos.append({"hilos": p, "t": t, "speedup": sp, "eficiencia": ef})
        print(f"{p:>6} {t:>12.6f} {sp:>8.2f}x {ef*100:>11.1f}%")
    return t_seq, puntos


# ---------- salidas ----------

def escribir_csv(resultados, path):
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["programa", "hilos", "tiempo_s", "speedup", "eficiencia"])
        for nombre, (t_seq, puntos) in resultados.items():
            w.writerow([nombre, "1(sec)", f"{t_seq:.6f}", "1.0000", "1.0000"])
            for d in puntos:
                w.writerow([nombre, d["hilos"], f"{d['t']:.6f}",
                            f"{d['speedup']:.4f}", f"{d['eficiencia']:.4f}"])


def escribir_markdown(resultados, path, nombre, reps):
    lineas = [f"# Resultados — {nombre}", "",
              f"Mediana de {reps} corridas · `Speedup = T_sec / T_p` · "
              f"`Eficiencia = Speedup / p`", ""]
    for prog, (t_seq, puntos) in resultados.items():
        lineas += [f"## {prog.capitalize()} — T secuencial = {t_seq:.4f} s", "",
                   "| Hilos | T (s) | Speedup | Eficiencia |",
                   "|------:|------:|--------:|-----------:|",
                   f"| 1 (sec) | {t_seq:.4f} | 1.00× | 100 % |"]
        for d in puntos:
            lineas.append(f"| {d['hilos']} | {d['t']:.4f} | "
                          f"{d['speedup']:.2f}× | {d['eficiencia']*100:.0f} % |")
        lineas.append("")
    with open(path, "w") as f:
        f.write("\n".join(lineas))


def graficar(resultados, path):
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("(matplotlib no instalado; se omiten las graficas: pip install matplotlib)")
        return None

    fig, (ax_s, ax_e) = plt.subplots(1, 2, figsize=(12, 4.5))
    maxh = 1
    for prog, (_, puntos) in resultados.items():
        hs = [d["hilos"] for d in puntos]
        maxh = max(maxh, *hs)
        ax_s.plot(hs, [d["speedup"] for d in puntos], "o-", label=prog)
        ax_e.plot(hs, [d["eficiencia"] * 100 for d in puntos], "o-", label=prog)

    ideal = list(range(1, maxh + 1))
    ax_s.plot(ideal, ideal, "k--", alpha=.4, label="ideal (lineal)")
    ax_s.set(title="Speedup vs hilos", xlabel="hilos", ylabel="speedup")
    ax_e.axhline(100, color="k", ls="--", alpha=.4, label="ideal (100 %)")
    ax_e.set(title="Eficiencia vs hilos", xlabel="hilos", ylabel="eficiencia (%)", ylim=(0, 115))
    for ax in (ax_s, ax_e):
        ax.grid(alpha=.3)
        ax.legend()

    fig.tight_layout()
    fig.savefig(path, dpi=120)
    plt.close(fig)
    return path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("nombre", nargs="?", default="equipo")
    ap.add_argument("--reps", type=int, default=3)
    ap.add_argument("--quick", action="store_true", help="reps=1, mas rapido")
    args = ap.parse_args()
    reps = 1 if args.quick else args.reps

    hilos = lista_hilos(os.cpu_count() or 1)
    print(f"Integrante: {args.nombre} | CPUs: {os.cpu_count()} | hilos: {hilos} | reps: {reps}")

    compilar()
    verificar_histograma()

    resultados = {}
    for nombre, sec, par in PROGRAMAS:
        resultados[nombre] = bench_programa(nombre, sec, par, hilos, reps)

    os.makedirs(IMG, exist_ok=True)
    csv_path = os.path.join(DOCS, f"resultados_{args.nombre}.csv")
    md_path = os.path.join(DOCS, f"resultados_{args.nombre}.md")
    png_path = os.path.join(IMG, f"{args.nombre}_metricas.png")

    escribir_csv(resultados, csv_path)
    escribir_markdown(resultados, md_path, args.nombre, reps)
    graficar(resultados, png_path)

    print("\nGenerado:")
    for p in (csv_path, md_path, png_path):
        if os.path.exists(p):
            print(f"  {os.path.relpath(p, RAIZ)}")


if __name__ == "__main__":
    main()
