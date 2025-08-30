/**
 * @class Timer
 * @brief 轻量级高精度计时器，支持毫秒/微秒/纳秒级耗时统计。
 *
 * 简介：
 * - 基于 std::chrono::high_resolution_clock 的计时工具。
 * - 提供模板化 elapsed<Duration>() 与便捷封装：elapsed_mill()、elapsed_micro()、elapsed_nano()。
 * - 通过 reset() 重置起点以复用同一实例；提供 Ptr 与 create() 便于共享管理。
 *
 * 使用示例：
 * @code
 * auto t = ff::Timer::create();
 * // ... do work ...
 * auto ms = t->elapsed();                 // 默认毫秒
 * auto us = t->elapsed<std::chrono::microseconds>();
 * t->reset();                             // 重置起点
 * @endcode
 *
 * 设计要点：
 * - 单位可定制：通过模板形参 Duration 指定任意 std::chrono::duration。
 * - 低开销：仅保存一个 time_point，查询时进行差值与单位转换。
 * - 非线程安全：如需跨线程访问，请自行加锁或为每线程创建独立计时器。
 *
 * 行为说明：
 * - 默认构造不会自动开始计时，m_begin 为默认值；建议在首次使用前调用 reset()。
 *   如需开箱即用，可在构造函数中将 m_begin 初始化为 high_resolution_clock::now()。
 *
 * 限制与注意：
 * - 精度与平台实现相关；high_resolution_clock 在不同平台可能等同于 steady_clock 或 system_clock。
 * - 仅用于本进程内的相对耗时统计，不用于跨进程或系统级时间对齐。
 *
 * 主要接口：
 * - void reset()                                        重置起点为当前时间
 * - template<typename Duration> int64_t elapsed() const 距离起点的耗时（以 Duration 计）
 * - int64_t elapsed_mill()/elapsed_micro()/elapsed_nano() 常用单位的便捷接口
 *
 * 成员说明：
 * - time_point<high_resolution_clock> m_begin  计时起点
 *
 * @author Chenchen Cao
 * @date 2025-08-29
 */

#pragma once

#include "../global/base.h"

namespace ff
{
	using namespace std::chrono;

	class Timer
	{
	public:
		using Ptr = std::shared_ptr<Timer>;
		static Ptr create()
		{
			return std::make_shared<Timer>();
		}

		Timer() :m_begin()
		{
			high_resolution_clock::now();
		}
		
		void reset()
		{
			m_begin = high_resolution_clock::now();
		}

		template<typename Duration = milliseconds>
		int64_t elapsed() const
		{
			return duration_cast<Duration>(high_resolution_clock::now() - m_begin).count();
		}

		int64_t elapsed_mill() const
		{
			return elapsed<milliseconds>();
		}

		int64_t elapsed_micro() const 
		{
			return elapsed<microseconds>();
		}

		int64_t elapsed_nano() const 
		{
			return elapsed<nanoseconds>();
		}



	private:
		time_point<high_resolution_clock> m_begin;

	};
}