// KDTreeMaison.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

template<typename coordType, size_t dimensions>
class Point
{
public:
    Point(std::array<coordType, dimensions> _coords) : coords(_coords) {}
    Point(std::initializer_list<coordType> list)
    {
        auto n = std::min(dimensions, list.size());
        std::copy_n(list.begin(), n, coords.begin());
    }

    coordType get(size_t i) const
    {
        return coords[i];
    }

    double distance(const Point& point) const
    {
        double dist = 0;

        for (auto i = 0; i < dimensions; ++i) {
            double temp = get(i) - point.get(i);
            dist += temp * temp;
        }

        return dist;
    }

private:
    std::array<coordType, dimensions> coords;
};

template<typename coordinate_type, size_t dimensions>
std::ostream& operator<<(std::ostream& out, const Point<coordinate_type, dimensions>& pt) {
    out << '(';
    for (size_t i = 0; i < dimensions; ++i) {
        if (i > 0)
            out << ", ";
        out << pt.get(i);
    }
    out << ')';
    return out;
}

template<typename coordType, size_t dimensions>
class KDTree
{
public:
    typedef Point<coordType, dimensions> ActualPoint;

private:
    struct Node
    {
        Node(const ActualPoint& _point) : point(_point), left(nullptr), right(nullptr) {}

        coordType get(size_t index) const {
            return point.get(index);
        }

        double distance(const ActualPoint& _point) const
        {
            return point.distance(_point);
        }

        ActualPoint point;
        Node* left;
        Node* right;
    };

    struct NodeComp
    {
        size_t index;

        NodeComp(size_t _index) : index(_index) {}

        bool operator()(const Node& nodeA, const Node& nodeB) const
        {
            return nodeA.point.get(index) < nodeB.point.get(index);
        }
    };

    Node* makeTree(size_t begin, size_t end, size_t index)
    {
        if (end <= begin) return nullptr;

        size_t median = begin + (end - begin) / 2;
        auto i = nodes.begin();

        /* https://en.cppreference.com/w/cpp/algorithm/nth_element
         * Algorithme de tri partiel réorganisant les élèments [first, last]
         * Parameter :
         * - First
         * - NTH
         * - Last
         * - Comparaison function
         */
        std::nth_element(i + begin, i + median, i + end, NodeComp(index));

        index = (index + 1) % dimensions;
        nodes[median].left = makeTree(begin, median, index);
        nodes[median].right = makeTree(median + 1, end, index);

        return &nodes[median];
    }

    void nearest(Node* _root, const ActualPoint& point, size_t index)
    {
        if (_root == nullptr) return;

        visitedPoint++;

        double dist = _root->distance(point);
        if (bestNode == nullptr || dist < bestDist)
        {
            bestDist = dist;
            bestNode = _root;
        }

        if (bestDist == 0) return;

        const double dX = _root->get(index) - point.get(index);
        index = (index + 1) % dimensions;

        nearest((dX > 0 ? _root->left : _root->right), point, index);
        if (dX * dX >= bestDist) return;
        nearest((dX > 0 ? _root->right : _root->left), point, index);
    }

    Node* root = nullptr;
    Node* bestNode = nullptr;
    double bestDist = 0;

    std::vector<Node> nodes;

public:
    KDTree(const KDTree&) = delete;
    KDTree& operator=(const KDTree&) = delete;

    template<typename Iterator>
    KDTree(Iterator begin, Iterator end) : nodes(begin, end)
    {
        root = makeTree(0, nodes.size(), 0);
    }

    double distance() const
    {
        return std::sqrt(bestDist);
    }

    const ActualPoint& nearest(const ActualPoint& point)
    {
        if (root == nullptr) throw std::logic_error("empty tree");

        bestNode = nullptr;
        visitedPoint = 0;
        bestDist = 0;
        nearest(root, point, 0);

        return bestNode->point;
    }

    size_t visitedPoint = 0;
};

int main()
{
    Point<int, 2> lstPoints[] = {
        {1, 2},
        {2, 3},
        {7, 3},
        {9, 3},
        {3, 6},
        {2, 4},
        {1, 12},
        {8, 0},
        {2, 1},
        {9, 9}
    };
    Point<int, 2> target = { 0, 4 };

    KDTree<int, 2> tree(std::begin(lstPoints), std::end(lstPoints));
    Point<int, 2> nearestNode = tree.nearest(target);

    std::cout << "Brut example data : " << std::endl;
    std::cout << "Target point : " << target << std::endl;
    std::cout << "Nearest point : " << nearestNode << std::endl;
    std::cout << "Distance : " << tree.distance() << std::endl;
    std::cout << "Visited node : " << tree.visitedPoint << std::endl;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.