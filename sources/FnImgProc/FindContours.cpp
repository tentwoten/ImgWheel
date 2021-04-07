#include "ImgProcess.h"
using namespace std;
namespace IMat
{
    //获取二值图的四联通域
    int FindRoot(int root, vector<int>& parent)
    {
        while (parent[root] != 0)
            root = parent[root];
        return root;
    }

    void MergeLabel(int root1, int root2, vector<int>& parent)
    {
        while (parent[root1] != 0)
            root1 = parent[root1];
        while (parent[root2] != 0)
            root2 = parent[root2];
        if (root1 < root2)
            parent[root2] = root1;
        if (root2 < root1)
            parent[root1] = root2;
    }

    int FindDomains(vector<vector<IMat::FnPoint2i>>& Widedomains, uint8_t* binImg, int w, int h)
    {
        if (binImg == nullptr)
            return -1;
        int cols = w;
        int rows = h;
        int nw = cols + 2;
        int nh = rows + 2;
        //把原图用黑边裱起来
        uint8_t* pad = new uint8_t[nh * nw]();
        for (int y = 1; y < rows; ++y)
        {
            uint8_t* psrc = binImg + (y - 1) * cols;
            uint8_t* pdst = pad + y * nw + 1;
            memcpy(pdst, psrc, cols * sizeof(uint8_t));
        }

        unsigned int* idMat = new unsigned int[nh * nw]();

        vector<int> clabel(1, 0);
        uint8_t* ptr = pad;
        int id = 0;
        //第一遍扫描
        for (int y = 1; y < nh - 1; ++y)
        {
            for (int x = 1; x < nw - 1; ++x)
            {
                int index = y * nw + x;
                int index_up = (y - 1) * nw + x;
                int index_left = y * nw + x - 1;
                if (ptr[index] == 255)
                {
                    if (ptr[index_up] == 0 && ptr[index_left] == 0)
                    {
                        idMat[index] = ++id;
                        clabel.push_back(0);
                    }
                    else if (ptr[index_up] == 0 && ptr[index_left] > 0)
                        idMat[index] = idMat[index_left];
                    else if (ptr[index_up] > 0 && ptr[index_left] == 0)
                        idMat[index] = idMat[index_up];
                    else
                    {
                        int upid = idMat[index_up];
                        int leftid = idMat[index_left];
                        if (upid != leftid)
                        {
                            idMat[index] = upid >= leftid ? leftid : upid;
                            MergeLabel(upid, leftid, clabel);
                        }
                        else
                            idMat[index] = upid;
                    }
                }
            }
        }

        for (int i = 0; i < clabel.size(); ++i)
        {
            if (clabel[i] != 0)
                clabel[i] = FindRoot(i, clabel);
        }

        Widedomains.resize(id + 1);
        for (int y = 1; y < nh - 1; ++y)
        {
            for (int x = 1; x < nw - 1; ++x)
            {
                int value = idMat[y * nw + x];
                if (value != 0)
                {
                    if (clabel[value] == 0)
                    {
                        Widedomains[value - 1].push_back(IMat::FnPoint2i(x - 1, y - 1));
                    }
                    else
                    {
                        int root = clabel[value];
                        Widedomains[root - 1].push_back(IMat::FnPoint2i(x - 1, y - 1));
                    }
                }
            }
        }
        delete[]pad;
        delete[]idMat;
        return 1;
    }

    int Get4BoundDomain(vector<vector<IMat::FnPoint2i>>& domains, uint8_t* binImg, int w, int h)
    {
        vector<vector<IMat::FnPoint2i>> wideDomains;
        FindDomains(wideDomains, binImg, w, h);
        if (domains.size())
            domains.clear();
        for (int i = 0, len = wideDomains.size(); i < len; ++i)
        {
            if (wideDomains[i].size() != 0)
                domains.push_back(wideDomains[i]);
        }
        return 1;
    }

    void FindCountour(vector<vector<IMat::FnPoint2i>>& countours, uint8_t* binImg, int w, int h, int method)
    {
        if (countours.size())
            countours.clear();
        if (binImg == nullptr)
            return;
        if (method == 0)
        {
            vector<vector<IMat::FnPoint2i>> wideDomains;
            FindDomains(wideDomains, binImg, w, h);
            int con_num = (int)wideDomains.size();
            for (int con_ind = 0; con_ind < con_num; ++con_ind)
            {
                int con_pt_num = (int)(wideDomains[con_ind].size());
                if (con_pt_num == 0)
                    continue;
                vector<IMat::FnPoint2i> row_vec;
                for (int pt_ind = 0; pt_ind < con_pt_num; ++pt_ind)
                {
                    IMat::FnPoint2i& pt = wideDomains[con_ind][pt_ind];
                    int y = pt.y;
                    int x = pt.x;
                    int index = y * w + x;
                    if (x > 0 && x < w - 1 && y > 0 && y < h - 1)
                    {
                        int left = index - 1;
                        int up = index - w;
                        int right = index + 1;
                        int down = index + w;
                        if (binImg[left] == 0 || binImg[right] == 0 || binImg[up] == 0 || binImg[down] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x == 0 && y > 0 && y < h - 1)
                    {
                        int up = index - w;
                        int right = index + 1;
                        int down = index + w;
                        if (binImg[right] == 0 || binImg[up] == 0 || binImg[down] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x == w - 1 && y > 0 && y < h - 1)
                    {
                        int left = index - 1;
                        int up = index - w;
                        int down = index + w;
                        if (binImg[left] == 0 || binImg[up] == 0 || binImg[down] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x > 0 && x < w - 1 && y == 0)
                    {
                        int left = index - 1;
                        int right = index + 1;
                        int down = index + w;
                        if (binImg[left] == 0 || binImg[right] == 0 || binImg[down] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x > 0 && x < w - 1 && y == h - 1)
                    {
                        int left = index - 1;
                        int up = index - w;
                        int right = index + 1;
                        if (binImg[left] == 0 || binImg[up] == 0 || binImg[right] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x == 0 && y == 0)
                    {
                        int right = index + 1;
                        int down = index + w;
                        if (binImg[right] == 0 || binImg[down] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x == w - 1 && y == 0)
                    {
                        int left = index - 1;
                        int down = index + w;
                        if (binImg[left] == 0 || binImg[down] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x == 0 && y == h - 1)
                    {
                        int right = index + 1;
                        int up = index - w;
                        if (binImg[right] == 0 || binImg[up] == 0)
                            row_vec.push_back(pt);
                    }
                    else if (x == w - 1 && y == h - 1)
                    {
                        int left = index + 1;
                        int up = index - w;
                        if (binImg[left] == 0 || binImg[up] == 0)
                            row_vec.push_back(pt);
                    }
                }
                countours.push_back(row_vec);
            }
        }
    }
};
