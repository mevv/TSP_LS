#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

const double INF = 999999;

enum class TYPE { NONE = -1, TSP, ATSP };
enum class EDGE_WEIGHT_TYPE { NONE = -1, EXPLICIT, EUC_2D, ATT };
enum class EDGE_WEIGHT_FORMAT { NONE = -1, FULL_MATRIX };
enum class SECTION { NONE = -1, EDGE_WEIGHT_SECTION, NODE_COORD_SECTION };

TYPE str2type(const std::string& str)
{
    if (str == "NAME")
        return TYPE::TSP;
    else if (str == "ATSP")
        return TYPE::ATSP;
    else
        return TYPE::NONE;
}

EDGE_WEIGHT_TYPE str2edgeWeightType(const std::string& str)
{
    if (str == "EXPLICIT")
        return EDGE_WEIGHT_TYPE::EXPLICIT;
    else if (str == "EUC_2D")
        return EDGE_WEIGHT_TYPE::EUC_2D;
    else if (str == "ATT")
        return EDGE_WEIGHT_TYPE::ATT;
    else
        return EDGE_WEIGHT_TYPE::NONE;
}

EDGE_WEIGHT_FORMAT str2edgeWeightFormat(const std::string& str)
{
    if (str == "FULL_MATRIX")
        return EDGE_WEIGHT_FORMAT::FULL_MATRIX;
    else
        return EDGE_WEIGHT_FORMAT::NONE;
}

SECTION str2section(const std::string& str)
{
    if (str == "EDGE_WEIGHT_SECTION")
        return SECTION::EDGE_WEIGHT_SECTION;
    else if (str == "NODE_COORD_SECTION")
        return SECTION::NODE_COORD_SECTION;
    else
        return SECTION::NONE;
}

int str2int(const std::string & str)
{
    int minus = 1;
    int x = 0;
    size_t pos = 0;

    if (str[pos] == '-')
    {
        minus = -1;
        ++pos;
    }
    else
        if (str[pos] == '+')
            ++pos;

    while (pos < str.length() && str[pos] >= '0' && str[pos] <= '9')
        x = x * 10 + str[pos++] - '0';

    x *= minus;

    return x;
}

//
//Left trim
//
std::string trim_left(const std::string& str)
{
  const std::string pattern = " \f\n\r\t\v";
  return str.substr(str.find_first_not_of(pattern));
}

//
//Right trim
//
std::string trim_right(const std::string& str)
{
  const std::string pattern = " \f\n\r\t\v";
  return str.substr(0,str.find_last_not_of(pattern) + 1);
}

//
//Left and Right trim
//
std::string trim(const std::string& str)
{
  return trim_left(trim_right(str));
}

class Tsp
{
public:
    //TSP() {}

    std::string getName() const { return m_name; }

    std::string getDescription() const { return m_comment; }

    int getSize() const { return m_size; }

    void showMatrix() { for (auto i : m_matrix) { std::cout << std::endl; for (auto j : i) std::cout << j << " "; } std::cout << std::endl; }

    void showInitial() { for (auto i : m_initial) { std::cout << std::endl; for (auto j : i) std::cout << j << " "; } std::cout << std::endl; }

    bool readFromFile(const std::string& filename)
    {
        std::ifstream ifs(filename.c_str());

        if (!ifs.is_open())
            return false;

        std::string tmp;

        while (getline(ifs, tmp))
        {
            std::string val;
            if (parseParam(tmp, "NAME", val))
                m_name = val;
            else if (parseParam(tmp, "COMMENT", val))
                m_comment = val;
            else if (parseParam(tmp, "TYPE", val))
                m_type = str2type(val);
            else if (parseParam(tmp, "DIMENSION", val))
                m_size = str2int(val);
            else if (parseParam(tmp, "EDGE_WEIGHT_TYPE", val))
                m_edgeWeightType = str2edgeWeightType(val);
            else if (parseParam(tmp, "EDGE_WEIGHT_TYPE", val))
                m_edgeWeightFormat = str2edgeWeightFormat(val);

            if (isEOF(tmp))
                break;

            if (isSection(tmp))
            {
                if (str2section(tmp) == SECTION::EDGE_WEIGHT_SECTION)
                    readMatrix(ifs);
                else if (str2section(tmp) == SECTION::NODE_COORD_SECTION)
                    readNodeCoord(ifs);
            }
        }

        return true;
    }

    bool readInitial(const std::string& filename, int num = 1)
    {
        std::ifstream ifs(filename);

        if (!ifs.is_open())
            return false;

        while(ifs.good())
        {
            m_initial.push_back(std::vector<double>());
            for (size_t i = 0; i < m_size; i++)
            {
                double tmp;
                ifs >> tmp;

                if (!ifs.good())
                {
                    m_initial.pop_back();
                    if (m_initial.empty())
                        return false;
                    else
                        return true;
                }

                m_initial.back().push_back(tmp);
            }
        }

        return true;
    }

    void solve()
    {
        for (size_t j = 0; j < m_initial.size(); j++)
        {
            bool isThereBetter = true;
            m_record = INF;
            m_path = m_initial[j];

            std::cout << "Initial lenght: " << getLenght(m_path) << std::endl;
            std::cout << "Initial: ";
            for (auto i : m_path) std::cout << i << " ";
            std::cout << std::endl;

            while (isThereBetter)
            {
                auto neighbors = getNeighbors();

                isThereBetter = false;
                for (size_t i = 0; i < neighbors.size(); i++)
                {
                    double lenght = INF;
                    if ((lenght = getLenght(neighbors[i])) < m_record)
                    {
                        m_record = lenght;
                        m_path = neighbors[i];
                        isThereBetter = true;
                        //std::cout << "New record: " << m_record << std::endl;
                        break;
                    }
                }
            }

            std::cout << "Record lenght: " << m_record << std::endl;
            std::cout << "Path: ";
            for (auto i : m_path) std::cout << i << " ";
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }

private:
    std::string m_name;
    std::string m_comment;

    TYPE m_type;
    EDGE_WEIGHT_TYPE m_edgeWeightType;
    EDGE_WEIGHT_FORMAT m_edgeWeightFormat;

    int m_size;

    double m_record;

    std::vector<std::vector<double> > m_matrix;
    std::vector<std::vector<double> > m_initial;
    std::vector<double> m_path;

    //parsing
    bool isSection(const std::string& str)
    {
        return str2section(str) != SECTION::NONE;
    }

    bool isEOF(const std::string& str)
    {
        if (str == "EOF")
            return true;

        return false;
    }

    bool parseSection(const std::string& str)
    {
        return str2section(str) != SECTION::NONE;
    }

    bool parseParam(const std::string& str, const std::string& name, std::string& result)
    {
        size_t pos;

        if (str.find(name) == std::string::npos)
            return false;

        if ((pos = str.find(':')) == std::string::npos)
            return false;

        std::string val(str, pos + 1, str.size());
        result = trim(val);

        return true;
    }

    bool readMatrix(std::ifstream& ifs)
    {
        for (size_t i = 0; i < m_size; i++)
        {
            m_matrix.push_back(std::vector<double>());
            for (size_t j = 0; j < m_size; j++)
            {
                double tmp;
                ifs >> tmp;

                m_matrix.back().push_back(tmp);
            }
        }

        return true;
    }

    bool readNodeCoord(std::ifstream& ifs)
    {
        std::vector<std::pair<double, double> > coord;
        int n;

        for (size_t i = 0; i < m_size; i++)
        {
            std::pair<double, double> tmp;
            ifs >> n;
            ifs >> tmp.first;
            ifs >> tmp.second;

            coord.push_back(tmp);
        }

        for (size_t i = 0; i < m_size; i++)
        {
            m_matrix.push_back(std::vector<double>());
            for (size_t j = 0; j < m_size; j++)
                if (i == j)
                    m_matrix.back().push_back(9999.0);
                else
                    m_matrix.back().push_back(dist(coord[i], coord[j]));
        }

        return true;
    }

    double dist(const std::pair<double, double>& a, const std::pair<double, double>& b)
    {
        return sqrt((b.first - a.first) * (b.first - a.first) + (b.second - a.second) * (b.second - a.second));
    }


    //solution
    double getLenght(const std::vector<double>& path)
    {
        double result = 0;

        for (size_t i = 0; i < m_size - 1; i++)
            result += m_matrix[path[i]][path[i + 1]];

        result += m_matrix[path[m_size - 1]][path[0]];

        return result;
    }

    std::vector<std::vector<double> > getNeighbors()
    {
        std::vector<std::vector<double> > result;

        for (size_t i = 0; i < m_size; i++)
        {
            for (size_t j = 0; j < m_size; j++)
            {
                if (j - i > 0)
                {
                    auto tmp = m_path;
                    std::swap(tmp[i], tmp[j]);
                    result.push_back(tmp);
                }
            }
        }

        //for (auto i : result) { std::cout << std::endl; for (auto j : i) std::cout << j << " "; std::cout << " " << getLenght(i); } std::cout << std::endl;

        return result;
    }
};



int main(int argc, char** argv)
{
    Tsp a;

    if (!a.readFromFile("../task/bays29.tsp"))
        std::cout << "Task read failed!" << std::endl;

    if (!a.readInitial("../initial/2_bays29.tsp.txt"))
        std::cout << "Initail failed!" << std::endl;

    std::cout << "Name: " << a.getName() << std::endl;
    std::cout << "Description: " << a.getDescription() << std::endl;
    std::cout << "Size: " << a.getSize() << std::endl;
    std::cout << "Matrix: ";
    a.showMatrix();
//    std::cout << "Initial: ";
//    a.showInitial();
    a.solve();


    return 0;
}
