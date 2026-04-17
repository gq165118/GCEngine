// metrics_demo.cpp
// A minimal C++ metrics demo: Gauge / Counter / Histogram / FastCompass / DBFastCompass
// with group-based registries and a periodic reporter.
// Compile: g++ -std=c++17 -O2 -pthread metrics_demo.cpp -o metrics_demo

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <random>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

using Clock = std::chrono::steady_clock;
using Ms = std::chrono::milliseconds;

// ---------- MetricName (name + tags) ----------
struct MetricName {
    std::string name;
    std::map<std::string, std::string> tags; // keep tags sorted by key

    static MetricName build(std::string n) {
        return MetricName{ std::move(n), {} };
    }
    MetricName& tagged(std::string k, std::string v) {
        tags[std::move(k)] = std::move(v);
        return *this;
    }
    std::string fullKey() const {
        // name|k1=v1|k2=v2
        std::string out = name;
        for (auto& kv : tags) {
            out += "|" + kv.first + "=" + kv.second;
        }
        return out;
    }
};

// ---------- Counter ----------
class Counter {
public:
    void inc(int64_t n = 1) { value_.fetch_add(n, std::memory_order_relaxed); }
    void dec(int64_t n = 1) { value_.fetch_sub(n, std::memory_order_relaxed); }
    int64_t value() const { return value_.load(std::memory_order_relaxed); }
private:
    std::atomic<int64_t> value_{ 0 };
};

// ---------- Gauge (pull-based) ----------
template<typename T>
class Gauge {
public:
    using Fn = std::function<T()>;
    explicit Gauge(Fn fn) : fn_(std::move(fn)) {}
    T value() const { return fn_ ? fn_() : T{}; }
private:
    Fn fn_;
};

// ---------- Histogram (simple, thread-safe) ----------
class Histogram {
public:
    void update(double v) {
        std::lock_guard<std::mutex> lk(mu_);
        samples_.push_back(v);
    }
    struct Snapshot {
        size_t count = 0;
        double min = NAN, max = NAN, mean = NAN, p50 = NAN, p95 = NAN, p99 = NAN, stddev = NAN;
    };
    Snapshot snapshot() const {
        std::lock_guard<std::mutex> lk(mu_);
        Snapshot s;
        s.count = samples_.size();
        if (s.count == 0) return s;
        std::vector<double> tmp = samples_;
        std::sort(tmp.begin(), tmp.end());
        s.min = tmp.front();
        s.max = tmp.back();
        s.mean = std::accumulate(tmp.begin(), tmp.end(), 0.0) / tmp.size();
        auto pct = [&](double p)->double {
            if (tmp.empty()) return NAN;
            double idx = (p / 100.0) * (tmp.size() - 1);
            size_t i = (size_t)std::floor(idx);
            size_t j = (size_t)std::ceil(idx);
            if (i == j) return tmp[i];
            double w = idx - i;
            return tmp[i] * (1.0 - w) + tmp[j] * w;
            };
        s.p50 = pct(50);
        s.p95 = pct(95);
        s.p99 = pct(99);
        // stddev
        double sumsq = 0.0;
        for (double v : tmp) sumsq += (v - s.mean) * (v - s.mean);
        s.stddev = std::sqrt(sumsq / tmp.size());
        return s;
    }
private:
    mutable std::mutex mu_;
    std::vector<double> samples_;
};

// ---------- FastCompass (label -> {count, sumRT}) ----------
class FastCompass {
public:
    void record(int64_t rt_ms, const std::string& label) {
        auto& b = buckets_[label]; // default-constructs atomics to 0
        b.count.fetch_add(1, std::memory_order_relaxed);
        b.sumRt.fetch_add(rt_ms, std::memory_order_relaxed);
    }
    struct BucketSnapshot {
        uint64_t count = 0;
        uint64_t sumRt = 0;
        double   qps = 0.0;   // computed vs previous snapshot
        double   avgRt = NAN;
    };
    struct Snapshot {
        std::map<std::string, BucketSnapshot> perLabel;
    };

    Snapshot snapshotAndResetDelta(double seconds) {
        // Not strictly resetting; we compute QPS by diff to previous totals.
        Snapshot s;
        for (auto& kv : buckets_) {
            auto& label = kv.first;
            auto& b = kv.second;
            uint64_t c = b.count.load(std::memory_order_relaxed);
            uint64_t sr = b.sumRt.load(std::memory_order_relaxed);
            uint64_t prevC = last_[label].count;
            uint64_t prevS = last_[label].sumRt;

            uint64_t dc = c - prevC;
            uint64_t ds = sr - prevS;

            BucketSnapshot bs;
            bs.count = c;
            bs.sumRt = sr;
            bs.qps = seconds > 0 ? (double)dc / seconds : 0.0;
            bs.avgRt = (dc > 0) ? (double)ds / (double)dc : NAN;
            s.perLabel[label] = bs;

            last_[label].count = c;
            last_[label].sumRt = sr;
        }
        return s;
    }
private:
    struct Bucket {
        std::atomic<uint64_t> count{ 0 };
        std::atomic<uint64_t> sumRt{ 0 }; // ms
    };
    std::unordered_map<std::string, Bucket> buckets_;
    struct Last { uint64_t count = 0, sumRt = 0; };
    std::unordered_map<std::string, Last> last_;
};

// ---------- DBFastCompass: a tagged FastCompass wrapper ----------
class DBFastCompass {
public:
    explicit DBFastCompass(FastCompass* fc,
        std::string dbType,
        std::string op,
        std::string pool)
        : fc_(fc), dbType_(std::move(dbType)), op_(std::move(op)), pool_(std::move(pool)) {}
    void record(int64_t rt_ms, bool success) {
        fc_->record(rt_ms, success ? "success" : "error");
    }
    // for demo, just expose tags:
    std::string tagString() const {
        return "dbType=" + dbType_ + ",op=" + op_ + ",pool=" + pool_;
    }
private:
    FastCompass* fc_;
    std::string dbType_, op_, pool_;
};

// ---------- Registry (grouped) ----------
class MetricRegistry {
public:
    // group + key string
    Counter& getCounter(const std::string& group, const MetricName& mn) {
        std::unique_lock<std::shared_mutex> lk(mu_);
        return counters_[group][mn.fullKey()];
    }
    template<typename T>
    void registerGauge(const std::string& group, const MetricName& mn, std::function<T()> fn) {
        std::unique_lock<std::shared_mutex> lk(mu_);
        gauges_[group][mn.fullKey()] = [fn = std::move(fn)]() { return std::to_string(fn()); };
    }
    Histogram& getHistogram(const std::string& group, const MetricName& mn) {
        std::unique_lock<std::shared_mutex> lk(mu_);
        return histograms_[group][mn.fullKey()];
    }
    FastCompass& getFastCompass(const std::string& group, const MetricName& mn) {
        std::unique_lock<std::shared_mutex> lk(mu_);
        return fastcompasses_[group][mn.fullKey()];
    }

    // Reporter: print a snapshot for all
    void report(std::ostream& os, double windowSec) {
        std::shared_lock<std::shared_mutex> lk(mu_);
        auto now = Clock::now();

        os << "\n================ METRICS REPORT (window " << windowSec << "s) ================\n";
        // COUNTERS
        for (auto& g : counters_) {
            os << "[Group=" << g.first << "] COUNTERS\n";
            for (auto& kv : g.second) {
                os << "  " << kv.first << ".count=" << kv.second.value() << "\n";
            }
        }
        // GAUGES
        for (auto& g : gauges_) {
            os << "[Group=" << g.first << "] GAUGES\n";
            for (auto& kv : g.second) {
                os << "  " << kv.first << ".value=" << kv.second() << "\n";
            }
        }
        // HISTOGRAMS
        for (auto& g : histograms_) {
            os << "[Group=" << g.first << "] HISTOGRAMS\n";
            for (auto& kv : g.second) {
                auto snap = kv.second.snapshot();
                os << "  " << kv.first
                    << " count=" << snap.count
                    << " min=" << snap.min
                    << " p50=" << snap.p50
                    << " p95=" << snap.p95
                    << " p99=" << snap.p99
                    << " max=" << snap.max
                    << " mean=" << snap.mean
                    << " stddev=" << snap.stddev
                    << "\n";
            }
        }
        // FASTCOMPASS
        for (auto& g : fastcompasses_) {
            os << "[Group=" << g.first << "] FASTCOMPASS\n";
            for (auto& kv : g.second) {
                auto snap = kv.second.snapshotAndResetDelta(windowSec);
                os << "  " << kv.first << "\n";
                for (auto& lb : snap.perLabel) {
                    os << "    label=" << lb.first
                        << " qps=" << std::fixed << std::setprecision(2) << lb.second.qps
                        << " avgRtMs=" << std::fixed << std::setprecision(2) << lb.second.avgRt
                        << " totalCount=" << lb.second.count
                        << "\n";
                }
            }
        }
        os << "=====================================================================\n";
    }

private:
    mutable std::shared_mutex mu_;
    // group -> key -> metric
    std::unordered_map<std::string, std::unordered_map<std::string, Counter>> counters_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<std::string()>>> gauges_;
    std::unordered_map<std::string, std::unordered_map<std::string, Histogram>> histograms_;
    std::unordered_map<std::string, std::unordered_map<std::string, FastCompass>> fastcompasses_;
};

// ---------- A tiny "MetricManager" facade ----------
struct MetricManager {
    static Counter& getCounter(MetricRegistry& r, const std::string& group, const MetricName& n) {
        return r.getCounter(group, n);
    }
    template<typename T>
    static void registerGauge(MetricRegistry& r, const std::string& group, const MetricName& n, std::function<T()> fn) {
        r.registerGauge<T>(group, n, std::move(fn));
    }
    static Histogram& getHistogram(MetricRegistry& r, const std::string& group, const MetricName& n) {
        return r.getHistogram(group, n);
    }
    static FastCompass& getFastCompass(MetricRegistry& r, const std::string& group, const MetricName& n) {
        return r.getFastCompass(group, n);
    }
};

// ---------- Demo workload ----------
int main() {
    MetricRegistry registry;

    // --- Gauges (queue watermarks, threadpool active, etc.)
    std::atomic<int> fakeQueue{ 0 };
    std::atomic<int> activeThreads{ 0 };

    MetricManager::registerGauge<int>(registry, "app",
        MetricName::build("queue.size").tagged("queue", "settlement"),
        [&]() { return fakeQueue.load(); });

    MetricManager::registerGauge<int>(registry, "app",
        MetricName::build("threadpool.active").tagged("pool", "biz"),
        [&]() { return activeThreads.load(); });

    // --- Counters
    auto& totalReq = MetricManager::getCounter(registry, "rpc",
        MetricName::build("user.get.count").tagged("method", "getUser").tagged("side", "provider"));

    auto& dbErrorCount = MetricManager::getCounter(registry, "db",
        MetricName::build("rds.query.error.count").tagged("dbType", "mysql").tagged("pool", "main"));

    // --- Histogram (payload size)
    auto& payloadHist = MetricManager::getHistogram(registry, "app",
        MetricName::build("api.response.payload_bytes"));

    // --- FastCompass (RPC / Cache)
    auto& rpcFC = MetricManager::getFastCompass(registry, "rpc",
        MetricName::build("user.get").tagged("method", "getUser").tagged("side", "provider"));

    auto& cacheFC = MetricManager::getFastCompass(registry, "cache",
        MetricName::build("user.profile.read").tagged("cache", "redis"));

    // --- DBFastCompass (wrap FastCompass with DB tags)
    auto& dbFCBase = MetricManager::getFastCompass(registry, "db",
        MetricName::build("rds.query").tagged("dbType", "mysql").tagged("op", "select").tagged("pool", "main"));
    DBFastCompass dbFC(&dbFCBase, "mysql", "select", "main");

    // --- Random generators
    std::mt19937_64 rng{ 1234567 };
    std::uniform_int_distribution<int> payloadDist(200, 10'000);
    std::bernoulli_distribution successDist(0.97);
    std::bernoulli_distribution cacheHitDist(0.80);
    std::uniform_int_distribution<int> rpcRT(2, 25);     // ms
    std::uniform_int_distribution<int> dbRT(3, 40);      // ms
    std::uniform_int_distribution<int> cacheRT(1, 4);    // ms
    std::uniform_int_distribution<int> qDelta(-3, 6);

    bool running = true;

    // --- Simulate RPC provider
    std::thread rpcT([&]() {
        while (running) {
            activeThreads.fetch_add(1);
            auto t0 = Clock::now();
            bool ok = successDist(rng);
            int  rt = rpcRT(rng);
            std::this_thread::sleep_for(Ms(rt));
            totalReq.inc();
            rpcFC.record(rt, ok ? "success" : "error");
            // response size histogram
            payloadHist.update((double)payloadDist(rng));
            activeThreads.fetch_sub(1);
            // small pause between requests
            std::this_thread::sleep_for(Ms(1));
        }
        });

    // --- Simulate DB queries
    std::thread dbT([&]() {
        while (running) {
            auto t0 = Clock::now();
            int rt = dbRT(rng);
            std::this_thread::sleep_for(Ms(rt));
            bool ok = successDist(rng);
            dbFC.record(rt, ok);
            if (!ok) dbErrorCount.inc();
            std::this_thread::sleep_for(Ms(3));
        }
        });

    // --- Simulate Cache reads (hit/miss/error)
    std::thread cacheT([&]() {
        while (running) {
            int rt = cacheRT(rng);
            std::this_thread::sleep_for(Ms(rt));
            bool hit = cacheHitDist(rng);
            bool ok = successDist(rng);
            if (hit) cacheFC.record(rt, "hit");
            else if (ok) cacheFC.record(rt, "success");
            else cacheFC.record(rt, "error");
            std::this_thread::sleep_for(Ms(1));
        }
        });

    // --- Fake queue fluctuation (gauge)
    std::thread qT([&]() {
        while (running) {
            int d = qDelta(rng);
            int cur = fakeQueue.load();
            int nxt = std::max(0, cur + d);
            fakeQueue.store(nxt);
            std::this_thread::sleep_for(Ms(50));
        }
        });

    // --- Reporter (every 2 seconds)
    const double windowSec = 2.0;
    std::thread reporter([&]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(windowSec * 1000)));
            registry.report(std::cout, windowSec);
        }
        });

    // Run for ~12 seconds
    std::this_thread::sleep_for(std::chrono::seconds(12));
    running = false;

    rpcT.join();
    dbT.join();
    cacheT.join();
    qT.join();
    reporter.join();

    std::cout << "\nDemo finished.\n";
    return 0;
}
