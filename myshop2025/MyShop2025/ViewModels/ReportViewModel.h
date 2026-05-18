#pragma once
#include "ViewModelBase.h"
#include "Models/ReportData.h"
#include <vector>

namespace winrt::MyShop2025::ViewModels
{
    struct ChartDataPoint
    {
        std::wstring Label;
        int64_t Value;
    };

    class ReportViewModel : public ViewModelBase
    {
    private:
        std::wstring m_period = L"day";
        std::wstring m_dateFrom;
        std::wstring m_dateTo;
        std::vector<ChartDataPoint> m_productSales;
        std::vector<ChartDataPoint> m_revenue;
        std::vector<ChartDataPoint> m_profit;

    public:
        ReportViewModel();

        std::wstring Period() const { return m_period; }
        void Period(const std::wstring& value)
        {
            if (m_period != value)
            {
                m_period = value;
                RaisePropertyChanged(L"Period");
                LoadReport();
            }
        }

        std::wstring DateFrom() const { return m_dateFrom; }
        void DateFrom(const std::wstring& value)
        {
            m_dateFrom = value;
            RaisePropertyChanged(L"DateFrom");
        }

        std::wstring DateTo() const { return m_dateTo; }
        void DateTo(const std::wstring& value)
        {
            m_dateTo = value;
            RaisePropertyChanged(L"DateTo");
        }

        std::vector<ChartDataPoint> ProductSales() const { return m_productSales; }
        void ProductSales(const std::vector<ChartDataPoint>& value)
        {
            m_productSales = value;
            RaisePropertyChanged(L"ProductSales");
        }

        std::vector<ChartDataPoint> Revenue() const { return m_revenue; }
        void Revenue(const std::vector<ChartDataPoint>& value)
        {
            m_revenue = value;
            RaisePropertyChanged(L"Revenue");
        }

        std::vector<ChartDataPoint> Profit() const { return m_profit; }
        void Profit(const std::vector<ChartDataPoint>& value)
        {
            m_profit = value;
            RaisePropertyChanged(L"Profit");
        }

        void LoadReport();
        void LoadReportForDate();
    };
}
