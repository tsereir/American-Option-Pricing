# Options américaines : théorie et algorithmes

## Plan du cours

1. Options américaines en temps discret
    1. Approche financière et raisonnement par arbitrage
    2. Equation de programmation dynamique
    3. Théorie de l'arrêt optimal et enveloppe de Snell
2. Options américaines en temps continu
    1. Enveloppe de Snell en temps continu
    2. Approche par EDP
    3. Méthodes de Monte-Carlo pour les options américaines : approche de Longstaff Schwartz et méthode de quantification
    4. Approche duale

## Projet

Le projet a pour objectif d'implémenter un outil de valorisation d'options américaines dans le modèle de Black Scholes multidimensionnel en utilisant l'algorithme de Longstaff Schwartz.

### Description du sujet

Vous implémenterez en C++ un outil de calcul des prix à l'instant $0$ dans le modèle de Black Scholes multidimensionnel d'options américaines dont le payoff à l'instant $t$ ne dépend que du cours $S_t$ des sous-jacents à cet instant (pas d'options path-dependent). Votre

- [Description du modèle de Black Scholes multidimensionnel](#modèle-de-black-scholes-multidimensionnel)
- [Liste des options à valoriser](#options)
- Votre code utilisera la librairie PNL. Pensez à lire le [chapitre 7 du manuel de PNL](https://pnlnum.github.io/pnl/manual-html/pnl-manualse7.html#x9-1040007) qui décrit les fonctionnalités offertes par la librairie pour mettre en œuvre diverses méthodes de regression polynomiale multivariée.
- Vous utiliserez l'algorithme de Longstaff Schwartz pour calculer le prix direct, le prix avec resimulation et la variance de ce dernier estimateur.

#### Modèle de Black Scholes multidimensionnel

Le modèle de Black Scholes de dimension $d \ge 1$ est défini par l'équation suivante sous la probabilité risque neutre. Pour $j \in \{1, \dots, d\}$

$$
\begin{align*}
  dS_{t,t} = S_{t,j} ( (r - \delta^j) dt + \sigma^j L_j dW_t)
\end{align*}
$$

où $r>0$ est le taux d'intérêt, $\delta \in \R^d$ est le taux de dividende, $B$ est un mouvement brownien à valeurs dans $\R^d$, $\sigma = (\sigma^1, \dots, \sigma^d)$ est le vecteur des volatilités supposées constantes et strictement positives et $L_j$ est la $j$-ème ligne de la matrice $L$ définie comme une racine carrée de la matrice de corrélation $\Gamma = LL^T$

$$
\begin{equation*}
  \Gamma = \begin{pmatrix}
    1 & \rho & \dots & \rho \\
    \rho & 1 &\ddots & \vdots \\
    \vdots&\ddots&\ddots& \rho \\
    \rho &\dots & \rho & 1
  \end{pmatrix}
\end{equation*}
$$

où $\rho \in ]-1 / (d-1), 1]$ pour garantir que la matrice $\Gamma$ est définie positive.

#### Options

Voici la liste des options que votre pricer devra être capable de valoriser

- Option panier (clé `basket`). Payoff à l'instant $t$

  ```math
  \begin{equation*}
    \left( \sum_{\ell=1}^d \lambda_d S_{t,\ell} - K \right)_+
  \end{equation*}
  ```

  où $\lambda_i \in \R$ et $K \in \R$. Le cas $K \le 0$ et les $\lambda_i$ négatifs permet de considérer une option de vente.

- Option géométrique (clé `geometric_put`). Payoff à l'instant $t$

  ```math
  \begin{equation*}
    \left(K - \left(\prod_{\ell=1}^d S_{t,\ell}\right)^{1/d}\right)_+
  \end{equation*}
  ```

  avec $K > 0$ le strike. C'est une option de vente sur un panier géométrique.

- Option performance sur panier (clé `bestof`). Payoff à l'instant $t$

  ```math
  \begin{equation*}
    \left(\max_{\ell=1,\dots,d} \lambda_d \, S_{t,\ell} - K \right)_+
  \end{equation*}
  ```

  où $\lambda_i \in \R$ et $K > 0$.

L'intérêt de l'option géométrique est que $\left(\prod_{\ell=1}^d S_{t,\ell}\right)^{1/d}$ suit une loi log-normale qui peut se ramener à un modèle de dimension 1
$$
\begin{align*}
  \hat S_0 e^{\left( (r - \delta - \hat \sigma^2 /2 ) t + \hat \sigma B_t \right)}
\end{align*}
$$
où $B$ est un mouvement brownien à valeurs réelles et
$$
\begin{align*}
  \hat S_0 = \left( \prod_{j=1}^d S_0^j \right)^{1/d}; \quad
  \hat \sigma = \frac{1}{d} \sqrt{\sigma^t \Gamma \sigma}; \quad
  \hat \delta  = \frac{1}{d} \sum_{j=1}^d \left( \delta^j + \frac{1}{2} (\sigma^j)^2 \right) - \frac{1}{2} (\hat \sigma)^2.
\end{align*}
$$
Une option vanille sur un panier géométrique de dimension quelconque peut donc se valoriser en utilisant une méthode uniquement disponible en petite dimension comme les méthodes d'arbre par exemple. L'option géométrique permet de valider une méthode numérique sur des exemples multidimensionnels.

Squelette du projet: [mcam-skel.tar.gz](document/mcam-skel.tar.gz) (le squelette contient quelques exemples de produits et leur prix).

#### Format des fichiers d'entrée

Les paramètres du problème sont spécifiés dans des fichiers au formation `JSON`. Voici la liste des paramètres

| Clé                              | Type   | Nom       |
|----------------------------------|--------|-----------|
| model size                       | int    | $d$       |
| spot                             | vector | $S_0$     |
| maturity                         | float  | $T$       |
| volatility                       | vector | $\sigma$  |
| interest rate                    | float  | $r$       |
| dividend rate                    | vector | $\delta$  |
| correlation                      | float  | $\rho$    |
| strike                           | float  | $K$       |
| option type                      | string |           |
| payoff coefficients              | vector | $\lambda$ |
| dates                            | int    | $N$       |
| MC iterations                    | int    | $M$       |
| degree for polynomial regression | int    |           |

Exemple de fichiers `JSON`

### Contraintes techniques

#### La librairie PNL

La librairie est téléchargeable depuis [https://github.com/pnlnum/pnl](https://github.com/pnlnum/pnl). La documentation de la librairie PNL est disponible au format [html](https://pnlnum.github.io/pnl/manual-html/pnl-manual.html) ou [pdf](https://pnlnum.github.io/pnl/pnl-manual.pdf).

#### Compilation

La compilation du projet se fera à l'aide de l'outil [cmake](http://www.cmake.org/documentation/) qui permet de générer aussi bien des Makefile Unix qu'un projet Visual. C'est un outil permettant de gérer la compilation multi-plateforme.

CMake est conçu pour lancer la compilation hors des sources de la manière suivante.

```
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/matieres/5MMPCPD/pnl ..
```

Il est possible de modifier d'autres variables utilisées par cmake

* `CMAKE_BUILD_TYPE` : `Debug` ou `Release`
* `CMAKE_CXX_COMPILER` : nom du compilateur C++
* `CMAKE_CXX_CFLAGS` : options du compilateur C++

### Instructions pour le rendu du projet

Le projet à réaliser en binôme est à rendre pour le vendredi 26 janvier 2024 à 16h sur TEIDE.

Votre code s'exécutera avec la commande

```
./mcam infile.json
```

et produira une sortie au format `JSON` générée au moyen de la classe `PricingResults`

```json
{
  "direct price": 9.87,
  "new simulation price": 9.6,
  "new simulation price stdDev": 0.15
}
```

où

- `direct price` est le prix obtenu directement par l'algorithme de Longstaff Schwartz;
- `new simulation price` est le prix obtenu par resimulation avec des nouvelles trajectoires;
- `new simulation price stdDev` est l'écart-type de l'estimateur du prix obtenu par resimulation.

**votre code ne doit rien afficher d'autre**. Cette sortie peut être obtenu avec le code

```cpp
std::cout << PricingResults(directPrice, newSimulationPrice, newSimulationPriceStdDev) << std::endl;
```

Le projet sera rendu sous la forme d'une archive au format `.tar.gz` dont le nom sera construit sur le format `Equipe_i.tar.gz` où `i` est le numéro de l'équipe tel que référencé sur _TEIDE_. Cette archive créera à l'extraction l'arborescence suivante

```
Equipe_i/
  |- AUTHORS
  |- CMakeLists.txt
  |- src/
  |- dat/
```

Pensez à vérifier la bonne structure de votre archive et sa compilation en utilisant le script [CheckMCAMArchive.py](document/CheckMCAMArchive.py)

```
python ./CheckMCAMArchive.py [--extract --destdir="chemin ou extraire l'archive" --build --pnldir="chemin vers pnl/build"] "chemin vers Equipe_i.tar.gz"
```

Si vous omettez la partie entre `[...]`, seule la structure de l'archive est vérifiée.
