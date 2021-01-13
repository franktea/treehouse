#include <fmt/format.h>

struct Complex
{
  double a;
  double b;
};

template<>
struct fmt::formatter<Complex>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }
  
  template<typename FormatContext>
  auto format(Complex const& number, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{0}+{1}i", number.a, number.b);
  }
};

int main()
{
  Complex number{1, 2};
  fmt::print("The number is {0}+{1}i.\n", number.a, number.b);
  fmt::print("The number is {}.\n", number);
}
