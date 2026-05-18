# T-04 — 修复 `Point` 哈希

> Phase-2 · 改动幅度：小 · 串行序号：4

## 任务描述

把 `Point` 哈希从 `hash<int>(x) ^ (hash<int>(y) << 1)` 换为 `hash_combine` 风格，降低对网格坐标 / 对称坐标的碰撞概率。

## 输入约束

- 可修改文件，仅限：
  - `src/core/Point.h`
- 新建文件，仅限：
  - `tests/test_point.cpp`
- 可修改文件，仅限：
  - `tests/CMakeLists.txt`（追加 `test_point.cpp` 到测试可执行文件源列表）
- **不得修改其他文件。**

## 输出约束

### 哈希算法

- 必须采用 `hash_combine` 风格，参考实现思路（**仅供参考，不直接抄录此处**）：
  - 计算 `h1 = std::hash<int>{}(p.x)`、`h2 = std::hash<int>{}(p.y)`。
  - 用 `h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2))` 之类混合。
- 哈希返回类型保持 `size_t`。
- `operator==` / `operator!=` 不得改动。

## 验收标准

- `tests/test_point.cpp` 至少包含以下用例（全部通过）：
  - `Point(a,b)` 与 `Point(b,a)`（`a != b`）哈希值 **不相等**：举例 `(1,2)` vs `(2,1)`，`(3,7)` vs `(7,3)`。
  - 把 `20×20 = 400` 个点 `(x,y)`（x∈[0,19], y∈[0,19]）插入 `std::unordered_set<Point>`，断言：
    - `set.size() == 400`。
    - `set.bucket_count() > 0`。
    - **遍历所有桶，`max_bucket_size <= 8`**（阈值放宽给出余量，目标是 ≤ 4）。
- 既有所有 `tests/` 用例不受影响（重跑全绿）。

## 不允许做的事

- 不允许引入 `boost`。
- 不允许引入 C++20 特性（`std::bit_cast`、`std::hash_combine` 等）。
- 不允许把 `hash` 定义搬到独立 cpp。

## 反问提示

- 若 `bucket_size` 阈值 8 在目标平台（MSVC v143）的默认 hash 实现下仍偶发超出，**反问主程**是否进一步收紧或放宽。
