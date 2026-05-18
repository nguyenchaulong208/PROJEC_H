#include "pch.h"
#include "OrdersPage.xaml.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::Views::implementation
{
    OrdersPage::OrdersPage()
    {
        InitializeComponent();
        m_viewModel = std::make_shared<ViewModels::OrdersViewModel>();
    }

    void OrdersPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        m_viewModel->LoadOrders();
        RefreshDisplay();
    }

    void OrdersPage::RefreshDisplay()
    {
        auto orderItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        for (const auto& order : m_viewModel->Orders())
        {
            auto orderCopy = order;
            orderItems.Append(winrt::box_value(orderCopy));
        }
        OrderList().ItemsSource(orderItems);
        PageText().Text((std::to_wstring(m_viewModel->CurrentPage() + 1) + L" / " +
            std::to_wstring(m_viewModel->TotalPages())).c_str());
    }

    void OrdersPage::SearchButton_Click(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto dateFrom = DateFromPicker().Date();
        auto dateTo = DateToPicker().Date();

        if (dateFrom)
        {
            auto dt = dateFrom.Value();
            std::wstringstream wss;
            wss << dt.Year << L"-" << std::setw(2) << std::setfill(L'0') << dt.Month
                << L"-" << std::setw(2) << std::setfill(L'0') << dt.Day;
            m_viewModel->DateFrom(wss.str());
        }
        if (dateTo)
        {
            auto dt = dateTo.Value();
            std::wstringstream wss;
            wss << dt.Year << L"-" << std::setw(2) << std::setfill(L'0') << dt.Month
                << L"-" << std::setw(2) << std::setfill(L'0') << dt.Day;
            m_viewModel->DateTo(wss.str());
        }
        m_viewModel->SearchByDate();
        RefreshDisplay();
    }

    void OrdersPage::CreateOrderButton_Click(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        ShowCreateOrderDialog();
    }

    void OrdersPage::OrderList_SelectionChanged(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&)
    {
    }

    void OrdersPage::DetailButton_Click(Windows::Foundation::IInspectable const& sender,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto order = winrt::unbox_value<Models::Order>(button.Tag());
        ShowOrderDetailDialog(order);
    }

    void OrdersPage::DeleteOrderButton_Click(Windows::Foundation::IInspectable const& sender,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto order = winrt::unbox_value<Models::Order>(button.Tag());
        m_viewModel->DeleteOrder(order.orderId);
        RefreshDisplay();
    }

    void OrdersPage::ShowCreateOrderDialog()
    {
        m_viewModel->StartCreateOrder();

        auto dialog = Controls::ContentDialog();
        dialog.Title(winrt::box_value(L"Tạo đơn hàng mới"));

        auto panel = StackPanel();
        panel.Spacing(8);

        auto productCombo = Controls::ComboBox();
        productCombo.Header(winrt::box_value(L"Chọn sản phẩm"));
        productCombo.PlaceholderText(L"Chọn sản phẩm...");
        auto prodItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        for (const auto& prod : m_viewModel->AvailableProducts())
        {
            if (prod.count > 0)
                prodItems.Append(winrt::box_value(prod.name + L" (" + std::to_wstring(prod.count) + L")"));
        }
        productCombo.ItemsSource(prodItems);

        auto addButton = Controls::Button();
        addButton.Content(winrt::box_value(L"Thêm vào giỏ"));
        addButton.Margin({ 0, 8, 0, 0 });

        auto cartList = Controls::ListView();
        cartList.MaxHeight(200);

        auto totalText = TextBlock();
        totalText.FontSize(18);
        totalText.FontWeight(Windows::UI::Text::FontWeights::Bold());
        totalText.Text(L"Tổng: 0 ₫");

        panel.Children().Append(productCombo);
        panel.Children().Append(addButton);
        panel.Children().Append(cartList);
        panel.Children().Append(totalText);

        dialog.Content(panel);
        dialog.PrimaryButtonText(L"Lưu đơn hàng");
        dialog.CloseButtonText(L"Hủy");

        dialog.PrimaryButtonClick([this](
            Controls::ContentDialog const&, Controls::ContentDialogButtonClickEventArgs const& args)
        {
            if (m_viewModel->CartItems().empty())
            {
                args.Cancel = true;
                return;
            }
            m_viewModel->SaveOrder();
            RefreshDisplay();
        });

        dialog.ShowAsync();
    }

    void OrdersPage::ShowOrderDetailDialog(const Models::Order& order)
    {
        auto dialog = Controls::ContentDialog();
        dialog.Title(winrt::box_value((L"Đơn hàng #" + std::to_wstring(order.orderId)).c_str()));

        auto panel = StackPanel();
        panel.Spacing(8);

        auto headerText = TextBlock();
        headerText.Text((L"Mã đơn: " + std::to_wstring(order.orderId)).c_str());
        headerText.FontWeight(Windows::UI::Text::FontWeights::Bold());

        auto statusText = TextBlock();
        std::wstring statusStr;
        switch (order.status)
        {
        case Models::OrderStatus::New: statusStr = L"Mới tạo"; break;
        case Models::OrderStatus::Paid: statusStr = L"Đã thanh toán"; break;
        case Models::OrderStatus::Cancelled: statusStr = L"Đã hủy"; break;
        }
        statusText.Text((L"Trạng thái: " + statusStr).c_str());

        auto totalText = TextBlock();
        totalText.Text((L"Tổng tiền: " + std::to_wstring(order.finalPrice) + L" ₫").c_str());
        totalText.FontSize(18);
        totalText.FontWeight(Windows::UI::Text::FontWeights::Bold());

        auto itemsList = Controls::ListView();
        itemsList.MaxHeight(200);
        auto itemItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        for (const auto& item : order.items)
        {
            std::wstring itemStr = item.productName + L" x" + std::to_wstring(item.quantity) +
                L" = " + std::to_wstring(item.totalPrice) + L" ₫";
            itemItems.Append(winrt::box_value(itemStr));
        }
        itemsList.ItemsSource(itemItems);

        panel.Children().Append(headerText);
        panel.Children().Append(statusText);
        panel.Children().Append(itemsList);
        panel.Children().Append(totalText);

        if (order.status == Models::OrderStatus::New)
        {
            auto payButton = Controls::Button();
            payButton.Content(winrt::box_value(L"Xác nhận thanh toán"));
            payButton.Margin({ 0, 8, 0, 0 });
            payButton.Click([this, order](Windows::Foundation::IInspectable const&,
                Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                m_viewModel->UpdateOrderStatus(order.orderId, Models::OrderStatus::Paid);
                RefreshDisplay();
            });
            panel.Children().Append(payButton);
        }

        dialog.Content(panel);
        dialog.PrimaryButtonText(L"Đóng");
        dialog.ShowAsync();
    }

    void OrdersPage::PrevButton_Click(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_viewModel->PreviousPage();
        RefreshDisplay();
    }

    void OrdersPage::NextButton_Click(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_viewModel->NextPage();
        RefreshDisplay();
    }
}
