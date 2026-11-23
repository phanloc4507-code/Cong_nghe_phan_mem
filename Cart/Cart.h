#ifndef CART_H
#define CART_H

#include <cstddef>
#include <string>
#include <vector>

struct CartItem {
    int productId{};
    std::string productName;
    long long unitPrice{};
    int quantity{};

    long long lineTotal() const;
};

enum class PaymentMethod { Cash, Online };
enum class OrderPaymentStatus { Unpaid, Paid };

struct ShippingPolicy {
    long long baseFee{30000};
    long long freeShippingFrom{500000};
};

struct OrderDraft {
    std::vector<CartItem> items;
    long long subtotal{};
    long long shippingFee{};
    long long total{};
    PaymentMethod paymentMethod{PaymentMethod::Cash};
    OrderPaymentStatus paymentStatus{OrderPaymentStatus::Unpaid};
};

class Cart {
public:
    bool addItem(int productId, const std::string& name, long long unitPrice, int quantity = 1);
    bool setQuantity(int productId, int quantity);
    bool removeItem(int productId);
    void clear();
    bool updatePrice(int productId, long long newUnitPrice);

    const std::vector<CartItem>& items() const { return items_; }
    bool empty() const { return items_.empty(); }
    std::size_t itemCount() const { return items_.size(); }
    long long subtotal() const;
    long long shippingFee(const ShippingPolicy& policy = {}) const;
    long long total(const ShippingPolicy& policy = {}) const;
    bool checkout(PaymentMethod method, OrderDraft& order,
                  const ShippingPolicy& policy = {});

private:
    std::vector<CartItem> items_;
};

const char* paymentMethodName(PaymentMethod method);
const char* paymentStatusName(OrderPaymentStatus status);

#endif
