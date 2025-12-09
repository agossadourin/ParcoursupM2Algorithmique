#include <Rcpp.h>
#include <unordered_map>
#include <vector>
#include <string>
using namespace Rcpp;

// Structure pour stocker le résultat d'un sous-problème
struct DPResult {
  double score;
  std::vector<int> assign;

  DPResult() : score(-INFINITY) {}
  DPResult(double s, const std::vector<int>& a) : score(s), assign(a) {}
};

// Fonction pour créer une clé de mémoisation
std::string make_key(int i, const std::vector<int>& cap_left) {
  std::string key = std::to_string(i) + "|";
  for (size_t j = 0; j < cap_left.size(); j++) {
    if (j > 0) key += ",";
    key += std::to_string(cap_left[j]);
  }
  return key;
}

// Variables globales pour éviter de passer trop de paramètres
NumericMatrix g_scores;
NumericMatrix g_notes;
NumericVector g_notes_min;
int g_n, g_m;
std::unordered_map<std::string, DPResult> g_memo;

// Fonction récursive de programmation dynamique
DPResult dp(int i, std::vector<int> cap_left) {
  // Condition terminale : tous les étudiants ont été traités
  if (i >= g_n) {
    return DPResult(0.0, std::vector<int>());
  }

  // Clé de mémoisation
  std::string key = make_key(i, cap_left);

  // Vérifier si déjà calculé
  auto it = g_memo.find(key);
  if (it != g_memo.end()) {
    return it->second;
  }

  // Option 0 : l'étudiant i reste non assigné
  DPResult best_result = dp(i + 1, cap_left);
  best_result.assign.insert(best_result.assign.begin(), 0);

  // Cas "affecté à une formation admissible"
  for (int j = 0; j < g_m; j++) {
    // Contrainte de note minimale
    if (g_notes(i, j) < g_notes_min[j]) continue;

    // Contrainte de capacité
    if (cap_left[j] <= 0) continue;

    // Créer nouvelle capacité
    std::vector<int> new_cap = cap_left;
    new_cap[j]--;

    // Résoudre le sous-problème
    DPResult res_next = dp(i + 1, new_cap);
    double cand_score = g_scores(i, j) + res_next.score;

    // Construire l'assignation candidate
    std::vector<int> cand_assign;
    cand_assign.push_back(j + 1);  // +1 car R indexe à partir de 1
    cand_assign.insert(cand_assign.end(), res_next.assign.begin(), res_next.assign.end());

    // Mettre à jour le meilleur résultat si nécessaire
    if (cand_score > best_result.score) {
      best_result.score = cand_score;
      best_result.assign = cand_assign;
    }
  }

  // Si aucune option valide (score reste -Inf), forcer à 0
  if (best_result.score == -INFINITY) {
    best_result.score = 0.0;
    best_result.assign = std::vector<int>(1, 0);
  }

  // Mémoriser le résultat
  g_memo[key] = best_result;

  return best_result;
}

// [[Rcpp::export]]
List solution_dp_cpp(NumericMatrix scores, IntegerVector capacities,
                     NumericMatrix notes, NumericVector notes_min) {

  // Initialiser les variables globales
  g_scores = scores;
  g_notes = notes;
  g_notes_min = notes_min;
  g_n = scores.nrow();
  g_m = scores.ncol();
  g_memo.clear();  // Vider la mémoisation des appels précédents

  // Convertir capacities en std::vector
  std::vector<int> cap_left(g_m);
  for (int j = 0; j < g_m; j++) {
    cap_left[j] = capacities[j];
  }

  // Lancer la programmation dynamique
  DPResult result = dp(0, cap_left);

  // Convertir le résultat en format R
  IntegerVector best_assign(result.assign.begin(), result.assign.end());

  return List::create(
    Named("best_score") = result.score,
    Named("best_assign") = best_assign
  );
}
