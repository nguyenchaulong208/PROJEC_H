#include "pch.h"
#include "ReportViewModel.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::ViewModels
{
    ReportViewModel::ReportViewModel()
    {
        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &tt);

        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d");
        m_dateTo = wss.str();

        tm.tm_mday = 1;
        auto firstDay = std::chrono::system_clock::from_time_t(_mktime32(&tm));
        tt = std::chrono::system_clock::to_time_t(firstDay);
        localtime_s(&tm, &tt);
        wss.str(L"");
        wss << std::put_time(&tm, L"%Y-%m-%d");
        m_dateFrom = wss.str();

        LoadReport();
    }

    void ReportViewModel::LoadReport()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (!dbSvc) return;

        auto data = dbSvc->GetReportData(m_period, m_dateFrom, m_dateTo);

        std::vector<ChartDataPoint> sales;
        for (const auto& [label, val] : data.productSales)
        {
            sales.push_back({ label, val });
        }
        ProductSales(sales);

        std::vector<ChartDataPoint> rev;
        for (const auto& [label, val] : data.revenue)
        {
            rev.push_back({ label, val });
        }
        Revenue(rev);

        std::vector<ChartDataPoint> prof;
        for (const auto& [label, val] : data.profit)
        {
            prof.push_back({ label, val });
        }
        Profit(prof);
    }

    void ReportViewModel::LoadReportForDate()
    {
        LoadReport();
    }
}
