#include <Rcpp.h>
#include <algorithm>
#include <vector>
using namespace Rcpp;

// Structure pour stocker un couple (étudiant, formation, score)
struct Pair {
  int student;    // indice étudiant (0-based)
  int formation;  // indice formation (0-based)
  double score;

  // Constructeur
  Pair(int s, int f, double sc) : student(s), formation(f), score(sc) {}
};

// Fonction de comparaison pour trier par score décroissant
bool comparePairs(const Pair& a, const Pair& b) {
  return a.score > b.score;
}

// [[Rcpp::export]]
List solution_greedy_cpp(NumericMatrix scores, IntegerVector capacities,
                         NumericMatrix notes, NumericVector notes_min) {

  int n = scores.nrow();
  int m = scores.ncol();

  // Créer la liste de tous les couples (i, j) admissibles
  std::vector<Pair> pairs;
  pairs.reserve(n * m);  // Réserver de l'espace pour éviter réallocations

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      // Vérifier si le couple respecte la note minimale
      if (notes(i, j) >= notes_min[j]) {
        pairs.push_back(Pair(i, j, scores(i, j)));
      }
    }
  }

  // Trier les couples par score décroissant
  std::sort(pairs.begin(), pairs.end(), comparePairs);

  // Construire l'affectation gloutonne
  IntegerVector assign(n, 0);      // 0 = non assigné
  IntegerVector cap_left = clone(capacities);
  double total_score = 0.0;

  for (size_t k = 0; k < pairs.size(); k++) {
    int i = pairs[k].student;
    int j = pairs[k].formation;

    // Si l'étudiant a déjà une formation, on saute
    if (assign[i] != 0) continue;

    // Si la formation est pleine, on saute
    if (cap_left[j] <= 0) continue;

    // Sinon, on affecte
    assign[i] = j + 1;  // +1 car R indexe à partir de 1
    cap_left[j]--;
    total_score += pairs[k].score;
  }

  return List::create(
    Named("best_score") = total_score,
    Named("best_assign") = assign
  );
}
