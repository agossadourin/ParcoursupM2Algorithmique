#include <Rcpp.h>
using namespace Rcpp;

// Variables globales pour stocker le meilleur résultat
double global_best_score;
IntegerVector global_best_assign;

// Fonction récursive DFS
void dfs(int i, int n, int m,
         double current_score,
         IntegerVector& assign_vec,
         IntegerVector& capacities_left,
         const NumericMatrix& scores,
         const NumericMatrix& notes,
         const NumericVector& notes_min,
         const NumericVector& best_score_per_student) {

  // Cas terminal : tous les étudiants ont été traités
  if (i >= n) {
    if (current_score > global_best_score) {
      global_best_score = current_score;
      global_best_assign = clone(assign_vec);
    }
    return;
  }

  // Borne supérieure (upper bound)
  double ub = current_score;
  for (int k = i; k < n; k++) {
    ub += best_score_per_student[k];
  }
  if (ub <= global_best_score) return;

  bool any_feasible = false;

  // Essayer chaque formation possible pour l'étudiant i
  for (int j = 0; j < m; j++) {
    if (notes(i, j) < notes_min[j]) continue;
    if (capacities_left[j] <= 0) continue;

    any_feasible = true;

    assign_vec[i] = j + 1;  // +1 car R indexe à partir de 1
    capacities_left[j]--;

    dfs(i + 1, n, m, current_score + scores(i, j), assign_vec,
        capacities_left, scores, notes, notes_min, best_score_per_student);

    // backtrack
    capacities_left[j]++;
  }

  // Si aucune formation possible : on laisse l'étudiant non assigné
  if (!any_feasible) {
    assign_vec[i] = 0;
    dfs(i + 1, n, m, current_score, assign_vec, capacities_left,
        scores, notes, notes_min, best_score_per_student);
  }
}

// [[Rcpp::export]]
List solution_bb_cpp(NumericMatrix scores, IntegerVector capacities,
                     NumericMatrix notes, NumericVector notes_min) {

  int n = scores.nrow();
  int m = scores.ncol();

  // Calcul du meilleur score possible par étudiant (upper bound)
  NumericVector best_score_per_student(n);
  for (int i = 0; i < n; i++) {
    double max_val = -INFINITY;
    for (int j = 0; j < m; j++) {
      if (notes(i, j) >= notes_min[j]) {
        if (scores(i, j) > max_val) {
          max_val = scores(i, j);
        }
      }
    }
    best_score_per_student[i] = (max_val == -INFINITY) ? 0.0 : max_val;
  }

  // Initialisation des variables globales
  global_best_score = -INFINITY;
  global_best_assign = IntegerVector(n, 0);

  IntegerVector assign_vec(n, 0);
  IntegerVector capacities_left = clone(capacities);

  // Lancement de la recherche
  dfs(0, n, m, 0.0, assign_vec, capacities_left, scores, notes,
      notes_min, best_score_per_student);

  return List::create(
    Named("best_score") = global_best_score,
    Named("best_assign") = global_best_assign
  );
}
