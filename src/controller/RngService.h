#ifndef SNAKE_CORE_RNGSERVICE_H
#define SNAKE_CORE_RNGSERVICE_H

#include <random>

/// \brief 可注入种子的随机数服务
/// \details 持有 std::mt19937 引擎，所有游戏随机性统一通过本服务获取。
///          seed(0) 使用 std::random_device 真随机，
///          非零种子用于 AI 训练 / 回放确定性复现。
class RngService {
public:
    RngService() {
        std::random_device rd;
        m_engine.seed(rd());
    }

    /// \brief 注入种子（s==0 时真随机）
    void seed(unsigned s) {
        m_engine.seed(s == 0 ? std::random_device{}() : s);
    }

    /// \brief 闭区间 [lo, hi] 均匀随机整数
    int intInRange(int loInclusive, int hiInclusive) {
        return std::uniform_int_distribution<int>(loInclusive, hiInclusive)(m_engine);
    }

    /// \brief 底层引擎引用（供高阶用法，本 Phase 外部不得自行使用 distribution）
    std::mt19937& engine() { return m_engine; }

private:
    std::mt19937 m_engine;
};

#endif // SNAKE_CORE_RNGSERVICE_H
