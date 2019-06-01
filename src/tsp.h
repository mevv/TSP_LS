#include <iostream>
#include <vector>

const double INF = 999999;
const size_t MAX_GFS_ITER = 100;
const double PENALTY_LAMBDA = 100;

enum class TYPE { NONE = -1, TSP, ATSP };
enum class EDGE_WEIGHT_TYPE { NONE = -1, EXPLICIT, EUC_2D, ATT };
enum class EDGE_WEIGHT_FORMAT { NONE = -1, FULL_MATRIX };
enum class SECTION { NONE = -1, EDGE_WEIGHT_SECTION, NODE_COORD_SECTION };
enum class ALGO { LS, GLS };

class TSP
{
public:
    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_comment; }
    int getSize() const { return m_size; }
    void showMatrix() { for (auto i : m_matrix) { std::cout << std::endl; for (auto j : i) std::cout << j << " "; } std::cout << std::endl; }
    void showInitial() { for (auto i : m_initial) { std::cout << std::endl; for (auto j : i) std::cout << j << " "; } std::cout << std::endl; }
    bool readFromFile(const std::string& filename);
    bool readInitial(const std::string& filename);
    // Local Search
    void LS(bool penalable = false);
    // Guided Local Search
    void GLS();
    void solve(ALGO algo = ALGO::LS, bool verbose = true);

private:
    std::string m_name;
    std::string m_comment;

    TYPE m_type;
    EDGE_WEIGHT_TYPE m_edgeWeightType;
    EDGE_WEIGHT_FORMAT m_edgeWeightFormat;

    size_t m_size;

    double m_record;

    std::vector<std::vector<double> > m_matrix;
    std::vector<std::vector<double> > m_initial;
    std::vector<std::vector<double> > m_penalties;
    std::vector<double> m_path;

    size_t m_iterations;

    //parsing
    bool isSection(const std::string& str);
    bool isEOF(const std::string& str);
    bool parseSection(const std::string& str);
    bool parseParam(const std::string& str, const std::string& name, std::string& result);
    bool readMatrix(std::ifstream& ifs);
    bool readNodeCoord(std::ifstream& ifs);
    double dist(const std::pair<double, double>& a, const std::pair<double, double>& b);

    //solving
    double getLenght(const std::vector<double>& path);
    double getPenalty(const std::vector<double>& path);
    void penalize();
    std::vector<std::vector<double>> getNeighbors();
};
