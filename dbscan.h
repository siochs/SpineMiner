#ifndef __DBSCAN_H__
#define __DBSCAN_H__

#include "kdtree/kdtree.h"
#include <utility>
#include <vector>

class Node : public std::pair<double, double>
{
public:
        Node(double x, double y) : std::pair<double, double>(y,x)
        {
                m_fVisited = false;
                m_nCluster = -1;
        }

        double GetX() const {return second;}
        double GetY() const {return first;}

        bool m_fVisited;
        int m_nCluster;

private:
        unsigned char color[3];
};

class DBScan
{
public:
     DBScan();
    ~DBScan();
    void InsertPoint(double x, double y);
    int RunDBScan(double dblEpsilon, int nMinPts);
    void Clear();

    std::vector<Node*> vecNodes;
    int nCluster;

private:
    bool GetNodesInRadius(Node* pt, double dblRadius, int nMinPts, std::vector<Node*>& rgpNodesFound);
    void ExpandCluster(std::vector<Node*>& rgp, int nCluster, double dblEpsilon, int nMinPts);
    kdtree* kdTree;
    Node* pt;

};

#endif // __DBSCAN_H__
