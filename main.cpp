#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace rng = std::ranges;
using namespace std;

class Domain {
public:
    explicit Domain(string value)
        : value_(std::move(value)) {

        rng::reverse(value_);

        if (value_.back() != '.') {
            value_.push_back('.');
        }
    }

    bool operator==(const Domain& other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Domain& other) const {
        return value_ != other.value_;
    }

    bool operator<(const Domain& other) const {
        return value_ < other.value_;
    }

    bool operator>(const Domain& other) const {
        return value_ > other.value_;
    }

    bool operator<=(const Domain& other) const {
        return value_ <= other.value_;
    }

    bool operator>=(const Domain& other) const {
        return value_ >= other.value_;
    }

    [[nodiscard]] bool IsSubdomain(const Domain& other) const {
        return value_.starts_with(other.value_);
    }

private:
    string value_;
};

class DomainChecker {
public:
    template<typename It>
    DomainChecker(It begin, It end)
        : forbidden_(begin, end) {

        rng::sort(forbidden_);

        // Удаляем поддомены, чтобы в списке запрещённых доменов ни один домен не был поддоменом другого
        const auto waste = rng::unique(forbidden_, [](const Domain& lhs, const Domain& rhs) {
            return rhs.IsSubdomain(lhs);
        });
        forbidden_.erase(waste.begin(), waste.end());
    }

    [[nodiscard]] bool IsForbidden(const Domain& domain) const {
        const auto nearest = rng::upper_bound(forbidden_, domain);

        if (nearest == forbidden_.begin()) {
            return false;
        }

        return domain.IsSubdomain(*prev(nearest));
    }

private:
    vector<Domain> forbidden_;
};

vector<Domain> ReadDomains(istream& input, size_t n) {
    vector<Domain> result;

    for (size_t i = 0; i < n; ++i) {
        string line;
        getline(input, line);

        result.emplace_back(std::move(line));
    }

    return result;
}

template<typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void TestDomain() {
    assert(Domain{"ya.ru"s}.IsSubdomain(Domain{"ru"}));
    assert(Domain{"ya.ru"s}.IsSubdomain(Domain{".ru"}));
    assert(Domain{".ya.ru"s}.IsSubdomain(Domain{"ru"}));
    assert(Domain{".ya.ru"s}.IsSubdomain(Domain{".ru"}));

    assert(Domain{"ya.google.ru"s}.IsSubdomain(Domain{"ru"}));
    assert(Domain{"ya.google.ru"s}.IsSubdomain(Domain{".ru"}));
    assert(Domain{".ya.google.ru"s}.IsSubdomain(Domain{"ru"}));
    assert(Domain{".ya.google.ru"s}.IsSubdomain(Domain{".ru"}));

    assert(Domain{"ya.google.ru"s}.IsSubdomain(Domain{"google.ru"}));
    assert(Domain{"ya.google.ru"s}.IsSubdomain(Domain{".google.ru"}));
    assert(Domain{".ya.google.ru"s}.IsSubdomain(Domain{"google.ru"}));
    assert(Domain{".ya.google.ru"s}.IsSubdomain(Domain{".google.ru"}));

    assert(!Domain{"ya.ru"s}.IsSubdomain(Domain{"u"}));
    assert(!Domain{"ya.ru"s}.IsSubdomain(Domain{".u"}));
    assert(!Domain{"ya.ru"s}.IsSubdomain(Domain{"rru"}));
    assert(!Domain{"ya.ru"s}.IsSubdomain(Domain{".rru"}));
    assert(!Domain{".ya.ru"s}.IsSubdomain(Domain{"u"}));
    assert(!Domain{".ya.ru"s}.IsSubdomain(Domain{".u"}));
    assert(!Domain{".ya.ru"s}.IsSubdomain(Domain{"rru"}));
    assert(!Domain{".ya.ru"s}.IsSubdomain(Domain{".rru"}));

    assert(Domain{"ya.ru"s}.IsSubdomain(Domain{"ya.ru"}));
    assert(Domain{"ya.ru"s}.IsSubdomain(Domain{".ya.ru"}));
    assert(Domain{".ya.ru"s}.IsSubdomain(Domain{"ya.ru"}));
    assert(Domain{".ya.ru"s}.IsSubdomain(Domain{".ya.ru"}));

    assert(Domain{"ya.ru"s} == Domain{"ya.ru"});
    assert(Domain{"ya.ru"s} == Domain{".ya.ru"});
    assert(Domain{".ya.ru"s} == Domain{"ya.ru"});
    assert(Domain{".ya.ru"s} == Domain{".ya.ru"});

    assert(Domain{"ru"s} == Domain{"ru"});
    assert(Domain{"ru"s} == Domain{".ru"});
    assert(Domain{".ru"s} == Domain{"ru"});
    assert(Domain{".ru"s} == Domain{".ru"});

    assert(Domain{""s} == Domain{""});

    assert(Domain{"ru"s} != Domain{"u"});
    assert(Domain{"ru"s} != Domain{""});
    assert(Domain{"ya.ru"s} != Domain{"google.ru"});
    assert(Domain{"ya.ru"s} != Domain{"ya.com"});
    assert(Domain{"ya.ru"s} != Domain{"ru.ya"});
    assert(Domain{"ur.ay"s} != Domain{"ya.ru"});
    assert(Domain{"ya.ru"s} != Domain{"yaru"});
}

void TestExample() {
    const std::vector<Domain> forbidden = {Domain{"gdz.ru"s},
                                           Domain{"maps.me"s},
                                           Domain{"m.gdz.ru"s},
                                           Domain{"com"s}};

    const DomainChecker checker(forbidden.begin(), forbidden.end());

    assert(checker.IsForbidden(Domain{"gdz.ru"s}));
    assert(checker.IsForbidden(Domain{"gdz.com"s}));
    assert(checker.IsForbidden(Domain{"m.maps.me"s}));
    assert(checker.IsForbidden(Domain{"alg.m.gdz.ru"s}));
    assert(checker.IsForbidden(Domain{"maps.com"s}));

    assert(!checker.IsForbidden(Domain{"maps.ru"s}));
    assert(!checker.IsForbidden(Domain{"gdz.ua"s}));
}

void TestDomainChecker() {
    const std::vector<Domain> forbidden = {Domain{"zzz"s},
                                           Domain{".zzz"s},
                                           Domain{"random.zzz"s},
                                           Domain{"yandex.random.zzz"s},
                                           Domain{"map.x"s},
                                           Domain{".x"s},
                                           Domain{"x"s},
                                           Domain{".x"s},
                                           Domain{"x"s},
                                           Domain{"map.x"s},
                                           Domain{"taboo.map.x"s},
                                           Domain{"maps.me"s},
                                           Domain{"math.gdz.ru"s},
                                           Domain{"ab.cd.ef.gh.ij.kl.mno.pqr.stu.vw.xyz"s},
                                           Domain{"biz"s}};

    const DomainChecker checker(forbidden.begin(), forbidden.end());

    assert(checker.IsForbidden(Domain{".zzz"s}));
    assert(checker.IsForbidden(Domain{"zzz"s}));
    assert(checker.IsForbidden(Domain{"gdz.zzz"s}));
    assert(checker.IsForbidden(Domain{"russian.gdz.zzz"s}));
    assert(checker.IsForbidden(Domain{"russian.lessons.gdz.zzz"s}));
    assert(checker.IsForbidden(Domain{"random.zzz"s}));
    assert(checker.IsForbidden(Domain{"yandex.random.zzz"s}));
    assert(checker.IsForbidden(Domain{"russian.lessons.gdz.x"s}));
    assert(checker.IsForbidden(Domain{"www.maps.me"s}));
    assert(checker.IsForbidden(Domain{"alg.math.gdz.ru"s}));
    assert(checker.IsForbidden(Domain{"z.ab.cd.ef.gh.ij.kl.mno.pqr.stu.vw.xyz"s}));
    assert(checker.IsForbidden(Domain{"maps.biz"s}));

    assert(!checker.IsForbidden(Domain{"yandex.random.yzzz"s}));
    assert(!checker.IsForbidden(Domain{"xzzz"s}));
    assert(!checker.IsForbidden(Domain{".xzzz"s}));
    assert(!checker.IsForbidden(Domain{"gdz.com"s}));
    assert(!checker.IsForbidden(Domain{"zzz.com"s}));
    assert(!checker.IsForbidden(Domain{"zzzx"s}));
    assert(!checker.IsForbidden(Domain{"zzz.me"s}));
    assert(!checker.IsForbidden(Domain{"www.map.me"s}));
    assert(!checker.IsForbidden(Domain{"www.maps.m"s}));
    assert(!checker.IsForbidden(Domain{"alg.gdz.ru"s}));
    assert(!checker.IsForbidden(Domain{"b.cd.ef.gh.ij.kl.mno.pqr.stu.vw.xyz"s}));
    assert(!checker.IsForbidden(Domain{"zab.cd.ef.gh.ij.kl.mno.pqr.stu.vw.xyz"s}));
    assert(!checker.IsForbidden(Domain{"z.ab.cd.ef.gh.ij.l.mno.pqr.stu.vw.xyz"s}));
    assert(!checker.IsForbidden(Domain{"biz.maps"s}));
    assert(!checker.IsForbidden(Domain{"gdz.ua"s}));
}

void TestReadDomains() {
    stringstream stream{"google.com\nya.ru\nmarket.biz\nde\nuk\nmath.gdz.ua\nwaste\ntaboo.forbidden"s};
    vector<Domain> test_a{Domain{"google.com"s},
                          Domain{"ya.ru"s},
                          Domain{"market.biz"s},
                          Domain{"de"s},
                          Domain{"uk"s},
                          Domain{"math.gdz.ua"s}};

    vector<Domain> test_b = ReadDomains(stream, 6);

    assert(test_a.size() == test_b.size());

    assert(test_a[0] == test_b[0]);
    assert(test_a[1] == test_b[1]);
    assert(test_a[2] == test_b[2]);
    assert(test_a[3] == test_b[3]);
    assert(test_a[4] == test_b[4]);
    assert(test_a[5] == test_b[5]);
}

int main() {
    TestDomain();
    TestExample();
    TestDomainChecker();
    TestReadDomains();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    const DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain: test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}