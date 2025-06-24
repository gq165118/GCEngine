/**
 * @class DriverInfo
 * @brief 用于记录整个渲染器在运行过程中的性能与资源使用情况。
 *
 * DriverInfo 类负责追踪与记录渲染器相关的关键统计指标，如：
 * - 几何体和纹理资源的使用数量
 * - 当前帧数与 draw call 次数
 * - 渲染出的三角形总数等
 *
 * 本类主要用于调试、性能分析和运行时监控，便于优化渲染流程与资源管理。
 *
 * @note 本类为轻量结构体，不涉及 GPU 资源，仅用于记录与查询渲染指标。
 * @author qiang.guo
 * @date 2025-06-20
 */



#pragma once
#include "../../global/base.h"

namespace ff
{
	//整个渲染器在渲染过程中的各项指标数据记录
	class DriverInfo
	{
	public:

		struct Memory
		{
			uint32_t m_geometries{ 0 };  //geomery的数量
			uint32_t m_textures{ 0 };	//贴图数量
		};

		struct Render
		{
			uint32_t	m_frame{ 0 }; //当前到了多少帧
			uint32_t	m_calls{ 0 };  //本帧调用了多少次drawCall
			uint32_t	m_triangles{ 0 }; 
		};

		using Ptr = std::shared_ptr<DriverInfo>;
		static Ptr create()
		{
			return std::make_shared<DriverInfo>();
		}

		DriverInfo() noexcept;
		~DriverInfo() noexcept;

		void update(uint32_t count, uint32_t glMode, uint32_t instanceCount) noexcept;

		void reset() noexcept;

	public:

		Memory m_memery{};
		Render m_render{};

	};
}