namespace Xapian {
    struct Weight {};
    struct BoolWeight : public Weight {
	BoolWeight() {}
    };
}
void f() {
    auto helper = [&](const Xapian::Weight& weight,
		      const char* name,
		      const char* params) {
    };
#define TEST_WEIGHTING_SCHEME(W, ...) helper(W(__VA_ARGS__), #W, "" #__VA_ARGS__)
    TEST_WEIGHTING_SCHEME(Xapian::BoolWeight);
#undef TEST_WEIGHTING_SCHEME
}
