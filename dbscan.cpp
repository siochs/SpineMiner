#include "dbscan.h"

DBScan::DBScan()
{
    kdTree = kd_create(2);
    nCluster = 0;
    return;
}

DBScan::~DBScan()
{
    kd_free(kdTree);
    return;
}

void DBScan::InsertPoint(double x, double y)
{
    pt = new Node(x, y);
    vecNodes.push_back(pt);
    double xy[2] = {x, y};
    kd_insert(kdTree, xy, pt);
    return;
}

bool DBScan::GetNodesInRadius(Node* pt, double dblRadius, int nMinPts, vector<Node*>& rgpNodesFound)
{
        double pos[2] = {pt->GetX(), pt->GetY()};
        kdres* res = kd_nearest_range(kdTree, pos, dblRadius);
        int number_near = kd_res_size(res);
        if (number_near >= nMinPts)
        {
                while (!kd_res_end(res))
                {
                        Node* ptNear = (Node*)kd_res_item(res, pos);
                        rgpNodesFound.push_back(ptNear);
                        kd_res_next(res);
                }
        }
        kd_res_free(res);
        return number_near >= nMinPts;
}

void DBScan::ExpandCluster(vector<Node*>& rgp, int nCluster, double dblEpsilon, int nMinPts)
{
        vector<Node*> rgpNeighbourhood;
        for (int i = 0; i < (int)rgp.size(); i++)
                if (!rgp[i]->m_fVisited)
                {
                        rgp[i]->m_nCluster = nCluster;
                        rgp[i]->m_fVisited = true;
                        rgpNeighbourhood.push_back(rgp[i]);
                }

        for (int i = 0; i < (int)rgpNeighbourhood.size(); i++)
        {
                Node* pNodeNear = rgpNeighbourhood[i];
                vector<Node*> rgpNeighbourhood2;
                if (GetNodesInRadius(pNodeNear, dblEpsilon, nMinPts, rgpNeighbourhood2))
                {
                        // append to rgpNeighbourhood items in rgpNeighbourhood2 that aren't already in rgpNeighbourhood
                        for (int j = 0; j < (int)rgpNeighbourhood2.size(); j++)
                        {
                                Node* pNode = rgpNeighbourhood2[j];
                                if (!pNode->m_fVisited)
                                        pNode->m_fVisited = true;
                                if (pNode->m_nCluster < 0)
                                {
                                        pNode->m_nCluster = nCluster;
                                        rgpNeighbourhood.push_back(pNode);
                                }
                        }
                }
        }
}

int DBScan::RunDBScan(double dblEpsilon, int nMinPts)
{
    nCluster = 0;
    for (vector<Node*>::const_iterator it = vecNodes.begin(); it != vecNodes.end(); it++)
    {
            Node* pNode = *it;
            if (!pNode->m_fVisited)
            {
                    pNode->m_fVisited = true;

                    vector<Node*> rgpNeighbourhood;
                    if (GetNodesInRadius(pNode, dblEpsilon, nMinPts, rgpNeighbourhood))
                    {
                            pNode->m_nCluster = nCluster;
                            pNode->m_fVisited = true;
                            ExpandCluster(rgpNeighbourhood, nCluster, dblEpsilon, nMinPts);
                            nCluster++;
                    }
            }
    }

    return nCluster;
}

void DBScan::Clear()
{
    vecNodes.clear();
}
