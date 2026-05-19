# Project MyShop 2025

## A. Yêu cầu chung

### A1. Tóm tắt yêu cầu
Tạo ra ứng dụng hỗ trợ chủ cửa hàng bán hàng.

### A2. Người dùng của hệ thống
- Hệ thống chỉ có một người dùng duy nhất là người chủ quán ăn nhỏ.

### A3. Kiến trúc chương trình
Chương trình có kiến trúc client–server, sử dụng database Postgres.

### A4. Luồng màn hình chính
- **LoginScreen** – Màn hình đăng nhập  
- **ConfigScreen** – Cấu hình địa chỉ server  
- **Dashboard** – Tổng quan hệ thống  
- **ProductsScreen** – Quản lý loại sản phẩm & sản phẩm  
- **OrdersScreen** – Quản lý đơn hàng  
- **ReportScreen** – Báo cáo kinh doanh  
- **SettingsScreen** – Cấu hình chương trình  

---

# A5. Lược đồ CSDL

## 1. Sơ đồ quan hệ tổng quan (chuyển từ hình vẽ)

```text
CATEGORY ───< PRODUCT

ORDER ───< ORDER_ITEM >─── PRODUCT
```

- Một CATEGORY có nhiều PRODUCT  
- Một ORDER có nhiều ORDER_ITEM  
- Một PRODUCT có nhiều ORDER_ITEM  

---

## 2. Lược đồ chi tiết

### CATEGORY
| Field        | Type    | Description |
|--------------|---------|-------------|
| category_id  | int     | Primary key |
| name         | string  | Tên loại    |
| description  | string  | Mô tả       |

---

### PRODUCT
| Field        | Type    | Description |
|--------------|---------|-------------|
| product_id   | int     | Primary key |
| sku          | string  | Mã SKU      |
| name         | string  | Tên sản phẩm |
| import_price | int     | Giá nhập    |
| count        | int     | Số lượng tồn |
| description  | string  | Mô tả       |

---

### ORDER
| Field        | Type      | Description |
|--------------|-----------|-------------|
| order_id     | int       | Primary key |
| created_time | DateTime  | Thời gian tạo |
| final_price  | int       | Tổng tiền đơn |

---

### ORDER_ITEM
| Field           | Type   | Description |
|-----------------|--------|-------------|
| order_item_id   | int    | Primary key |
| quantity        | int    | Số lượng mua |
| unit_sale_price | float  | Giá bán 1 đơn vị |
| total_price     | int    | Thành tiền |
| order_id        | int    | FK → ORDER.order_id |
| product_id      | int    | FK → PRODUCT.product_id |

---

## Một số lưu ý
- Thiết kế CSDL chỉ là gợi ý, học viên có thể tùy biến.  
- Giá sản phẩm dùng integer là đủ (max 4 tỉ).  

---

# V B. Các chức năng cơ sở (5 điểm)

## B1. Đăng nhập (0.25 điểm)
- Tự động đăng nhập nếu có thông tin lưu từ lần trước  
- Thông tin đăng nhập phải được mã hóa  
- Hiển thị thông tin phiên bản  
- Cho phép cấu hình server từ màn hình Config  

---

## B2. Dashboard tổng quan hệ thống (0.5 điểm)

Dashboard hiển thị:
- Tổng số sản phẩm  
- Top 5 sản phẩm sắp hết hàng (số lượng < 5)  
- Top 5 sản phẩm bán chạy  
- Tổng số đơn hàng trong ngày  
- Tổng doanh thu trong ngày  
- Chi tiết 3 đơn hàng gần nhất  
- Biểu đồ doanh thu theo ngày trong tháng hiện tại  

---

## B3. Quản lý sản phẩm – Products (1.25 điểm)

- Xem danh sách sản phẩm theo loại  
- Xem chi tiết → Xóa / Sửa  
- Phân trang  
- Sắp xếp theo 1 tiêu chí  
- Lọc theo khoảng giá  
- Tìm kiếm theo từ khóa  
- Thêm loại sản phẩm & thêm sản phẩm  
- Import từ Excel hoặc Access  

### Yêu cầu dữ liệu mẫu
- Ít nhất 3 loại sản phẩm  
- Mỗi loại ≥ 22 sản phẩm  
- Mỗi sản phẩm ≥ 3 hình  
- Dữ liệu mẫu không cần thật nhưng nên giống thật  

---

## B4. Quản lý đơn hàng – Orders (1.5 điểm)

- Tạo đơn hàng  
- Xóa / cập nhật đơn hàng  
- Xem danh sách đơn hàng (phân trang)  
- Xem chi tiết đơn hàng  
- Tìm kiếm đơn hàng theo ngày  

### Trạng thái đơn hàng
- Mới tạo  
- Đã thanh toán  
- Đã hủy  

---

## B5. Báo cáo thống kê – Report (1 điểm)

Mục tiêu:
1. Biết tình trạng hệ thống  
2. Theo dõi tình hình kinh doanh  

Chức năng:
- Xem sản phẩm & số lượng bán theo ngày/tuần/tháng/năm (biểu đồ đường)  
- Báo cáo doanh thu & lợi nhuận theo ngày/tuần/tháng/năm (biểu đồ cột/bánh)  

---

## B6. Cấu hình chương trình (0.25 điểm)

- Chọn số lượng sản phẩm mỗi trang: 5/10/15/20  
- Lưu lại màn hình chính lần cuối mở  

---


# C. Các chức năng tự chọn (5 điểm)

- Auto save khi tạo đơn hàng / thêm sản phẩm (0.25)  
- Responsive layout (0.5)  
- Kiến trúc plugin mở rộng (1)  
- Khuyến mãi giảm giá (1)  
- Obfuscator chống dịch ngược (0.25)  
- Chế độ dùng thử 15 ngày (0.5)  
- Backup / restore database (0.25)  
- GraphQL API (1)  
- MVVM (0.5)  
- Dependency Injection (0.5)  
- Phân quyền admin / moderator / sale (0.5)  
- Hoa hồng bán hàng theo KPI (0.25)  
- Quản lý khách hàng (0.5)  
- Test case kiểm thử (0.5)  
- In đơn hàng ra PDF/XPS (0.5)  
- Sắp xếp theo nhiều tiêu chí (0.5)  
- Tìm kiếm nâng cao (1)  
- Onboarding hướng dẫn sử dụng (0.5)  

