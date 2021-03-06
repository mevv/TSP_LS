#include "tsp.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <chrono>

namespace {

std::string ltrim(const std::string& str)
{
  const std::string pattern = " \f\n\r\t\v";
  return str.substr(str.find_first_not_of(pattern));
}

std::string rtrim(const std::string& str)
{
  const std::string pattern = " \f\n\r\t\v";
  return str.substr(0,str.find_last_not_of(pattern) + 1);
}

std::string trim(const std::string& str)
{
  return ltrim(rtrim(str));
}

TYPE str2type(const std::string& str)
{
    if (str == "TSP")
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

}

bool TSP::readFromFile(const std::string& filename)
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
        else if (parseParam(tmp, "DIMENSION", val))
            m_size = std::stoi(val);
        else if (parseParam(tmp, "EDGE_WEIGHT_TYPE", val))
            m_edgeWeightType = str2edgeWeightType(val);
        else if (parseParam(tmp, "TYPE", val))
            m_type = str2type(val);
        else if (parseParam(tmp, "EDGE_WEIGHT_FORMAT", val))
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

    for (size_t i = 0; i < m_size; i++)
        m_penalties.push_back(std::vector<double>(m_size));

    return true;
}

bool TSP::readInitial(const std::string& filename)
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

void TSP::LS(bool penalable)
{
    bool isThereBetter = true;

    m_iterations = 0;
    m_record = INF;

    if (m_path.size() != m_size)
        return ;

    while (isThereBetter)
    {
        m_iterations++;
        auto neighbors = getNeighbors();

        isThereBetter = false;
        for (size_t i = 0; i < neighbors.size(); i++)
        {
            double lenght = INF;
            if ((lenght = getLenght(neighbors[i]) + (penalable ? getPenalty(neighbors[i]) : 0)) < m_record)
            {
                m_record = lenght;
                m_path = neighbors[i];
                isThereBetter = true;
                //std::cout << "New record: " << m_record << std::endl;
                break;
            }
        }
    }
}

void TSP::GLS()
{
    for (auto& i : m_penalties)
        for (auto& j : i)
            j = 0;

    LS();

    auto initial = m_path;
    auto best = m_path;
    double bestLenght = getLenght(m_path);

    size_t iterations = 0;
    while (iterations < MAX_GFS_ITER)
    {
        m_path = initial;
        penalize();
        LS(true);

        iterations++;

        if (getLenght(m_path) < bestLenght)
        {
            best = m_path;
            bestLenght = getLenght(m_path);
            break;
        }
    }

    m_iterations = iterations;
    m_path = best;
    m_record = bestLenght;
}

void TSP::solve(ALGO algo, bool verbose)
{
    for (size_t j = 0; j < m_initial.size(); j++)
    {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        int time = 0;

        m_path = m_initial[j];

        if (verbose)
        {
            std::cout << "Initial lenght: " << getLenght(m_path) << std::endl;
            std::cout << "Initial: ";
            for (auto i : m_path) std::cout << i << " ";
            std::cout << std::endl;
        }

        start = std::chrono::system_clock::now();

        if (algo == ALGO::LS)
            LS();
        else if (algo == ALGO::GLS)
            GLS();
        else
        {
            std::cout << "Wrong algorithm: " << (int)algo << std::endl;
            return ;
        }

        end = std::chrono::system_clock::now();
        time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

        if (verbose)
        {
            std::cout << "Iterations: " << m_iterations << std::endl;
            std::cout << "Elapsed time: " << time << " ms" << std::endl;
            std::cout << "Record lenght: " << m_record << std::endl;
            std::cout << "Path: ";
            for (auto i : m_path) std::cout << i << " ";
            std::cout << std::endl << std::endl;
        }
    }
}

bool TSP::isSection(const std::string& str)
{
    return str2section(str) != SECTION::NONE;
}

bool TSP::isEOF(const std::string& str)
{
    return str == "EOF";
}

bool TSP::parseSection(const std::string& str)
{
    return str2section(str) != SECTION::NONE;
}

bool TSP::parseParam(const std::string& str, const std::string& name, std::string& result)
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

bool TSP::readMatrix(std::ifstream& ifs)
{
    for (size_t i = 0; i < m_size; i++)
    {
        m_matrix.push_back(std::vector<double>());
        for (size_t j = 0; j < m_size; j++)
        {
            double tmp;
            ifs >> tmp;

            if (!ifs)
                return false;

            m_matrix.back().push_back(tmp);
        }
    }

    return true;
}

bool TSP::readNodeCoord(std::ifstream& ifs)
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

double TSP::dist(const std::pair<double, double>& a, const std::pair<double, double>& b)
{
    if (m_edgeWeightType == EDGE_WEIGHT_TYPE::ATT)
    {
        double r = sqrt(((b.first - a.first) * (b.first - a.first) + (b.second - a.second) * (b.second - a.second)) / 10.0);
        return round(r) < r ? round(r) + 1.0 : round(r);
    }

    return sqrt((b.first - a.first) * (b.first - a.first) + (b.second - a.second) * (b.second - a.second));
}

double TSP::getLenght(const std::vector<double>& path)
{
    double result = 0;

    for (size_t i = 0; i < m_size - 1; i++)
        result += m_matrix[path[i]][path[i + 1]];

    result += m_matrix[path[m_size - 1]][path[0]];

    return result;
}

double TSP::getPenalty(const std::vector<double>& path)
{
    double result = 0;

    for (size_t i = 0; i < m_size - 1; i++)
        result += m_penalties[path[i]][path[i + 1]];

    result += m_penalties[path[m_size - 1]][path[0]];

    return result * PENALTY_LAMBDA;
}

void TSP::penalize()
{
    double lenght = getLenght(m_path);

    double maxUtility = 0;
    size_t index = 0;

    for (size_t i = 0; i < m_size; i++)
    {
        if (lenght / (1 + m_penalties[i][i != m_size - 1 ? i + 1 : 0]) > maxUtility)
        {
            maxUtility = lenght / (1 + m_penalties[i][i != m_size - 1 ? i + 1 : 0]);
            index = i;
        }
    }

    m_penalties[index][index != m_size - 1 ? index + 1 : 0]++;
}

std::vector<std::vector<double>> TSP::getNeighbors()
    {
        std::vector<std::vector<double> > result;

        for (size_t i = 0; i < m_size; i++)
        {
            for (size_t j = 0; j < m_size; j++)
            {
                if (i != j)
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
