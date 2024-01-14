#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

template<class T>
std::string ConvertToString(T value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

class ExportHeaderModel
{
public:
    ExportHeaderModel(std::string strDepId, std::string strExportData) : m_strDepId(strDepId), m_strExportDate(strExportData) {

    }

    std::string getDepId() {
        return m_strDepId;
    }

    std::string getExportDate() {
        return m_strExportDate;
    }
private:
    std::string m_strDepId;     // 对账单的部分id
    std::string m_strExportDate;    // 对账单的导出日期
};

class ExportDataModel
{
public:
    ExportDataModel(std::string strTransId, double Quantity) : m_strTransId(strTransId), m_Quantity(Quantity) {

    }

    std::string getTransId() {
        return m_strTransId;
    }
    double getQuantity() {
        return m_Quantity;
    }
private:
    std::string m_strTransId;   // 交易Id
    double      m_Quantity;
};

class ExportFooterModel
{
public:
    ExportFooterModel(std::string exportUser) : m_exportUser(exportUser) {}

    std::string getExportUser() {
        return m_exportUser;
    }
private:
    std::string m_exportUser;
};

// 考虑到我们在一个对账周期，可能有多笔的交易记录，我们用vector
class ExportToTxtHelper
{
public:
    void doExport(ExportHeaderModel& ehm, vector<ExportDataModel*>& edmCollection, ExportFooterModel& efm);
};

void ExportToTxtHelper::doExport(ExportHeaderModel& ehm, vector<ExportDataModel*>& edmCollection, ExportFooterModel& efm)
{
    // 用一个字符串拼接
    std::string strTemp = "";
    // 第一步：写文件头
    strTemp += ehm.getDepId() + ", " + ehm.getExportDate() + "\n";

    // 第二步：写文件体数据
    for (vector<ExportDataModel*>::iterator it = edmCollection.begin(); it != edmCollection.end(); it++) {
        strTemp += (*it)->getTransId() + ":" + ConvertToString((*it)->getQuantity()) + "\n";
    }

    // 第三步：拼接文件尾部
    strTemp += efm.getExportUser() + "\n";

    std::cout << strTemp;

}

int main(int argc, char* argv[])
{
    ExportHeaderModel* pEhm  = new ExportHeaderModel("南京1支行", "6月1日");
    ExportDataModel*   pEdm1 = new ExportDataModel("1", 10000.00f);
    ExportDataModel*   pEdm2 = new ExportDataModel("2", 20000.00f);
    std::vector<ExportDataModel*> myVec;
    myVec.emplace_back(pEdm1);
    myVec.emplace_back(pEdm2);
    ExportFooterModel* pEfm  = new ExportFooterModel("xcj");

    ExportToTxtHelper helper;
    helper.doExport(*pEhm, myVec, *pEfm);
    return 0;
}