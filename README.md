# ParcoursupM2Algorithmique 

Package R pour résoudre des problèmes d'affectation étudiants-formations inspirés de Parcoursup, avec des implémentations haute performance en C++.

[![R](https://img.shields.io/badge/R-276DC3?style=flat&logo=r&logoColor=white)](https://www.r-project.org/)
[![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Rcpp](https://img.shields.io/badge/Rcpp-276DC3?style=flat)](https://www.rcpp.org/)

---

##  Problème

Attribuer des **formations** à des **étudiants** en maximisant un score combinant préférences et mérite académique :

$$\max \sum_{i=1}^{n} \sum_{j=1}^{m} (\alpha \cdot p_{ij} + \beta \cdot n_i) \cdot x_{ij}$$

**Contraintes** :
- Un étudiant reçoit au plus une formation
- Chaque formation a une capacité maximale
- Note minimale requise pour chaque formation

---

## Installation

```r
# Installer remotes si nécessaire
if (!requireNamespace("remotes", quietly = TRUE))
  install.packages("remotes")

# Installer le package
remotes::install_github("username/ParcoursupM2Algorithmique")
```

**Prérequis** : R ≥ 4.0.0, Rcpp ≥ 1.0.0, compilateur C++

---

## 🔧 Méthodes

Chaque méthode est disponible en **R** et en **C++ (Rcpp)** :

| Méthode | Complexité | Optimal | Fonction R | Fonction C++ |
|---------|------------|---------|-----------|--------------|
| **Naïve** | O((m+1)ⁿ) | O | `solution_naive_simple()` | — |
| **Branch & Bound** | O((m+1)ⁿ)* | O| `solution_bb()` | `solution_bb_cpp()` |
| **Gloutonne** | O(nm log nm) | N | `solution_greedy()` | `solution_greedy_cpp()` |
| **Prog. Dynamique** | O(n·m·∏cⱼ) | O | `solution_dp()` | `solution_dp_cpp()` |

*En pratique beaucoup plus rapide grâce à l'élagage

---

## 💡 Exemple d'utilisation

```r
library(ParcoursupM2Algorithmique)

# Matrice des scores (8 étudiants × 3 formations)
scores <- matrix(runif(24, 100, 500), nrow = 8, ncol = 3)

# Capacités des formations
capacities <- c(3, 2, 3)

# Notes des étudiants (matrice 8×3, même note répétée)
notes_mat <- matrix(rep(runif(8, 10, 20), times = 3), nrow = 8, ncol = 3)

# Notes minimales requises
notes_min <- c(10, 12, 8)

# Résolution avec Branch & Bound (C++)
result <- solution_bb_cpp(scores, as.integer(capacities), notes_mat, notes_min)

print(result$best_score)   # Score optimal
print(result$best_assign)  # Affectations (0 = non assigné, 1..m = formation)
```

---

## ⚡ Performances

Les versions C++ sont **21× à 433× plus rapides** que les versions R :

| Méthode | Speedup C++/R |
|---------|---------------|
| Branch & Bound | **433×** |
| Prog. Dynamique | **44×** |
| Gloutonne | **21×** |

---

## Auteurs

**AGOSSADOU Guerin** • **CHAQUIQ ELBADRE Youssef** • **MOUISSOU Lina**

---
