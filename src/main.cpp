#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>


enum TYPE { TSP, ATSP };
enum EDGE_WEIGHT_TYPE { EXPLICIT, EUC_2D, ATT };
enum EDGE_WEIGHT_FORMAT { FULL_MATRIX };

TYPE str2type(const std::string& str)
{
    if (str == "NAME")
        return TYPE::TSP;
    else if (str == "ATSP")
        return TYPE::ATSP;
}

EDGE_WEIGHT_TYPE str2edgeWeightType(const std::string& str)
{
    if (str == "EXPLICIT")
        return EDGE_WEIGHT_TYPE::EXPLICIT;
    else if (str == "EUC_2D")
        return EDGE_WEIGHT_TYPE::EUC_2D;
    else if (str == "ATT")
        return EDGE_WEIGHT_TYPE::ATT;
}

EDGE_WEIGHT_FORMAT str2edgeWeightFormat(const std::string& str)
{
    if (str == "FULL_MATRIX")
        return EDGE_WEIGHT_FORMAT::FULL_MATRIX;
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

    void showMatrix() { for (auto i : m_matrix) { std::cout << std::endl; for (auto j : i) std::cout << j << " "; } }

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

            if (isSection(tmp))
                if (!parseSection(ifs, tmp))
                    return false;
        }

        return true;
    }

    bool readInitial(const std::string& filename, int num = 1)
    {

        return true;
    }


private:
    std::string m_name;
    std::string m_comment;

    TYPE m_type;
    EDGE_WEIGHT_TYPE m_edgeWeightType;
    EDGE_WEIGHT_FORMAT m_edgeWeightFormat;

    int m_size;

    double m_record;

    std::vector<std::vector<double> > m_matrix;//for EXPLICIT
    std::vector<double> m_path;

    //parsing
    bool isSection(const std::string& str)
    {
        if (str == "EOF")
            return true;
        else if (str == "EDGE_WEIGHT_SECTION")
            return true;
        else if (str == "NODE_COORD_SECTION")
            return true;

        return false;
    }

    bool parseSection(std::ifstream& ifs, const std::string& str)
    {
        if (str == "EOF")
            return true;

        if (str == "EDGE_WEIGHT_SECTION")
            return readMatrix(ifs);
        else if (str == "NODE_COORD_SECTION")
            return readNodeCoord(ifs);

        return false;
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
};



int main(int argc, char** argv)
{
    Tsp a;

    a.readFromFile("../task/br17.atsp");

    std::cout << "Name: " << a.getName() << std::endl;
    std::cout << "Description: " << a.getDescription() << std::endl;
    std::cout << "Size: " << a.getSize() << std::endl;
    std::cout << "Matrix: " << std::endl;
    a.showMatrix();


    return 0;
}
