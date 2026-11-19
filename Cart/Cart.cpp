#include "Cart.h"

#include <algorithm>
#include <limits>

long long CartItem::lineTotal() const {
    return unitPrice * quantity;
}

bool Cart::addItem(int productId, const std::string& name, long long unitPrice, int quantity) {
    if (productId <= 0 || name.empty() || unitPrice < 0 || quantity <= 0) return false;

    auto it = std::find_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.productId == productId;
    });
    if (it != items_.end()) {
        if (it->quantity > std::numeric_limits<int>::max() - quantity) return false;
        it->quantity += quantity;
        it->productName = name;
        it->unitPrice = unitPrice;
    } else {
        items_.push_back({productId, name, unitPrice, quantity});
    }
    return true;
}

bool Cart::setQuantity(int productId, int quantity) {
    auto it = std::find_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.productId == productId;
    });
    if (it == items_.end()) return false;
    if (quantity <= 0) items_.erase(it);
    else it->quantity = quantity;
    return true;
}

bool Cart::removeItem(int productId) {
    auto it = std::find_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.productId == productId;
    });
    if (it == items_.end()) return false;
    items_.erase(it);
    return true;
}

void Cart::clear() {
    items_.clear();
}

bool Cart::updatePrice(int productId, long long newUnitPrice) {
    if (newUnitPrice < 0) return false;
    auto it = std::find_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.productId == productId;
    });
    if (it == items_.end()) return false;
    it->unitPrice = newUnitPrice;
    return true;
}

long long Cart::subtotal() const {
    long long result = 0;
    for (const auto& item : items_) result += item.lineTotal();
    return result;
}

long long Cart::shippingFee(const ShippingPolicy& policy) const {
    if (items_.empty() || policy.baseFee <= 0) return 0;
    if (policy.freeShippingFrom > 0 && subtotal() >= policy.freeShippingFrom) return 0;
    return policy.baseFee;
}

long long Cart::total(const ShippingPolicy& policy) const {
    return subtotal() + shippingFee(policy);
}

bool Cart::checkout(PaymentMethod method, OrderDraft& order, const ShippingPolicy& policy) {
    if (items_.empty()) return false;

    order.items = items_;
    order.subtotal = subtotal();
    order.shippingFee = shippingFee(policy);
    order.total = order.subtotal + order.shippingFee;
    order.paymentMethod = method;
    order.paymentStatus = (method == PaymentMethod::Online)
        ? OrderPaymentStatus::Paid
        : OrderPaymentStatus::Unpaid;
    clear();
    return true;
}

const char* paymentMethodName(PaymentMethod method) {
    return method == PaymentMethod::Cash ? "Tien mat" : "Online (gia lap)";
}

const char* paymentStatusName(OrderPaymentStatus status) {
    return status == OrderPaymentStatus::Paid ? "Da thanh toan" : "Chua thanh toan";
}
