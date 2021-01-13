#include <fmt/format.h>
#include <functional>

using namespace std;
using namespace fmt;

struct Node {
    int value;
    Node* left;
    Node* right;
    Node(int v, Node* l = nullptr, Node* r = nullptr):value(v), left(l), right(r) {}
};

template <>
struct formatter<Node*> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Node* p, FormatContext &ctx)
  {
    fmt::memory_buffer out;

    std::function<void(const Node*)> f = [&out, &f](const Node* root) {
        if(!root) return;
        format_to(out, "{}", "(");
        format_to(out, "{}", root->value);
        if(root->left) f(root->left);
        if(root->right) {
            if(! root->left) format_to(out, "{}", "()");
            f(root->right);
        }
        format_to(out, "{}", ")");
    };

    f(p);

    return format_to(ctx.out(), "{}", to_string(out));
  }
};

int main() {
    Node* root = new Node(1, new Node(2, nullptr, new Node(4)), new Node(3, new Node(5, new Node(7), new Node(8)), new Node(6)));
    fmt::print("{}\n", root);
}
