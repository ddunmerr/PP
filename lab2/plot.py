#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Строит T(N), S(N), E(N) из results.csv.
Требует: pandas, matplotlib.
Запуск: python plot.py results.csv
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt

CSV = sys.argv[1] if len(sys.argv) > 1 else "results.csv"

df = pd.read_csv(CSV)

# Восстанавливаем cores по позиции: 4 блока по 80 строк
repeats, n_values = 5, 16
block = repeats * n_values  # 80
df["cores"] = (df.index // block) + 1

# Медиана по (cores, threads)
med = df.groupby(["cores", "threads"], as_index=False)["ms"].median()

# T1, S, E
t1 = med[med["threads"] == 1].set_index("cores")["ms"].to_dict()
med["T1"] = med["cores"].map(t1)
med["S"]  = med["T1"] / med["ms"]
med["E"]  = med["S"] / med["threads"]

# Печать таблиц
print("=== T(N), ms ===")
print(med.pivot(index="threads", columns="cores", values="ms").to_string())
print("\n=== S(N) ===")
print(med.pivot(index="threads", columns="cores", values="S").round(3).to_string())
print("\n=== E(N) ===")
print(med.pivot(index="threads", columns="cores", values="E").round(3).to_string())

print("\n=== p по Амдалу ===")
for k in [2, 3, 4]:
    smax = med[med["cores"] == k]["S"].max()
    p = k * (1 - 1 / smax) / (k - 1)
    print(f"  {k} ядер: S_max = {smax:.3f}  ->  p ≈ {p:.3f}")

# Графики
N = list(range(1, 17))
cores_list = [1, 2, 3, 4]
colors = {1: "#1f77b4", 2: "#ff7f0e", 3: "#2ca02c", 4: "#d62728"}

fig, axes = plt.subplots(1, 3, figsize=(18, 5.5))

ax = axes[0]
for k in cores_list:
    sub = med[med["cores"] == k].sort_values("threads")
    ax.plot(sub["threads"], sub["ms"], marker="o", color=colors[k], label=f"{k} cores")
ax.set_xlabel("N (threads)"); ax.set_ylabel("T(N), ms"); ax.set_title("Время T(N)")
ax.set_xticks(N); ax.grid(True, alpha=0.3); ax.legend()

ax = axes[1]
for k in cores_list:
    sub = med[med["cores"] == k].sort_values("threads")
    ax.plot(sub["threads"], sub["S"], marker="o", color=colors[k], label=f"{k} cores")
ax.plot(N, N, "--", color="gray", label="идеал S=N")
ax.set_xlabel("N (threads)"); ax.set_ylabel("S(N) = T(1)/T(N)"); ax.set_title("Ускорение S(N)")
ax.set_xticks(N); ax.grid(True, alpha=0.3); ax.legend()

ax = axes[2]
for k in cores_list:
    sub = med[med["cores"] == k].sort_values("threads")
    ax.plot(sub["threads"], sub["E"], marker="o", color=colors[k], label=f"{k} cores")
ax.axhline(1.0, linestyle="--", color="gray", label="идеал E=1")
ax.set_xlabel("N (threads)"); ax.set_ylabel("E(N) = S(N)/N"); ax.set_title("Эффективность E(N)")
ax.set_xticks(N); ax.grid(True, alpha=0.3); ax.legend()

plt.tight_layout()
plt.savefig("graphs.png", dpi=150)
plt.show()
print("\nСохранено: graphs.png")