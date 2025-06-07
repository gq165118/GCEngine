/**
 * @file gl_define.h
 * @brief 本文件定义了OpenGL相关的常量、类型转换工具函数以及输入事件（键盘/鼠标）映射规则。
 *
 * 主要功能包括：
 * - 键盘与鼠标动作枚举及其GLFW按键码映射；
 * - 各类OpenGL使用的数据类型、纹理格式、缓冲类型、混合方式、深度测试方式的枚举封装；
 * - 枚举值与OpenGL常量之间的转换函数；
 * - 提供统一的数据类型（如DataType）到大小、OpenGL类型码等的转换；
 * - 一些常用纹理、材质、顶点属性名称的常量定义。
 *
 * Example usage:
 * ```
 * // 获取float对应的OpenGL类型码
 * GLuint glType = ff::toGL(ff::toDataType<float>());
 *
 * // 获取左键按下事件类型
 * auto leftDown = std::get<0>(ff::MouseActionMap.at(GLFW_MOUSE_BUTTON_LEFT));
 * ```
 *
 * @note 本文件不依赖运行时状态，所有函数均为纯函数或常量表达式，可安全多线程调用。
 * @author qiang.guo
 * @date 2025-06-07
 */

#pragma once
#include "base.h"
#include <stb_image.h>

namespace ff
{
	using KeyboardAction = uint64_t;

	static constexpr KeyboardAction WKey = 1;
	static constexpr KeyboardAction SKey = 2;
	static constexpr KeyboardAction AKey = 3;
	static constexpr KeyboardAction DKey = 4;

	static constexpr KeyboardAction KeyNONE = 31;

#define DOWN	0
#define UP		1

	using KeyboardMapType = std::unordered_map<uint32_t, KeyboardAction>;

	//预设好四个按键
	static const KeyboardMapType KeyboardActionMap =
	{
		{GLFW_KEY_W, WKey},
		{GLFW_KEY_S, SKey},
		{GLFW_KEY_A, AKey},
		{GLFW_KEY_D, DKey}
	};

	enum  class MouseAction : uint8_t
	{
		LeftDown,
		RightDown,
		MiddleDown,
		LeftUp,
		RightUp,
		MiddleUp,
		NONE,
	};

	//key:当前哪个按键发生了反应；value：对应up or down, tuple拥有顺序,std::get<0>拿到的就是第一个mouseAction
	using MouseActionMapType = std::unordered_map<uint32_t, std::tuple<MouseAction, MouseAction>>;

	static const MouseActionMapType mouseActionMap =
	{
		{GLFW_MOUSE_BUTTON_LEFT, {MouseAction::LeftDown, MouseAction::LeftUp}},
		{GLFW_MOUSE_BUTTON_LEFT, {MouseAction::RightDown, MouseAction::RightUp}},
		{GLFW_MOUSE_BUTTON_LEFT, {MouseAction::MiddleDown, MouseAction::MiddleUp}},
	};

	enum class DataType
	{
		UnsignedByteType,
		ByteType,
		Int32Type,
		UInt32Type,
		FloatType
	};

	template<typename T>
	static DataType toDataType()
	{
		if (typeid(T) == typeid(float))
		{
			return DataType::FloatType;
		}

		if (typeid(T) == typeid(int)) {
			return DataType::Int32Type;
		}

		if (typeid(T) == typeid(char)) {
			return DataType::ByteType;
		}

		if (typeid(T) == typeid(unsigned char)) {
			return DataType::UnsignedByteType;
		}

		if (typeid(T) == typeid(uint32_t)) {
			return DataType::UInt32Type;
		}

		return DataType::FloatType;
	}

	static unsigned int toGL(const DataType& value)
	{
		switch (value)
		{
		case DataType::UnsignedByteType:
			return GL_UNSIGNED_BYTE;
		case DataType::FloatType:
			return GL_FLOAT;
		case DataType::ByteType:
			return GL_BYTE;
		case DataType::Int32Type:
			return GL_INT;
		case DataType::UInt32Type:
			return GL_UNSIGNED_INT;
		default:
		{
			std::cout << "bad" << std::endl;
			return 0;
		}
		}
	}

	static size_t toSize(const DataType& value)
	{
		switch (value)
		{
		case DataType::UnsignedByteType:
			return sizeof(unsigned char);
		case DataType::FloatType:
			return sizeof(float);
		case DataType::ByteType:
			return sizeof(char);
		case DataType::Int32Type:
			return sizeof(int);
		case DataType::UInt32Type:
			return sizeof(uint32_t);
		default:
			return 0;
		}
	}

	//events
	static const std::string DISPOSE = "dispose";

	//material
	namespace MaterialName
	{
		static const std::string Material = "Material";
		static const std::string MeshBasicMaterial = "MeshBasicMaterial";
		static const std::string MeshPhongMaterial = "MeshPhongMaterial";
		static const std::string CubeMaterial = "CubeMaterial";
		static const std::string DepthMaterial = "DepthMaterial";
	};

	//geometry
	enum class BufferAllocType
	{
		StaticDrawBuffer,
		DynamicDrawBuffer
	};

	static uint32_t toGL(const BufferAllocType& value)
	{
		switch (value)
		{
		case BufferAllocType::StaticDrawBuffer:
			return GL_STATIC_DRAW;
		case BufferAllocType::DynamicDrawBuffer:
			return GL_DYNAMIC_DRAW;
		default:
			return 0;
		}
	}

	enum class BufferType
	{
		ArrayBuffer,
		IndexBuffer
	};

	static uint32_t toGL(const BufferType& value)
	{
		switch (value)
		{
		case BufferType::ArrayBuffer:
			return GL_ARRAY_BUFFER;
		case BufferType::IndexBuffer:
			return GL_ELEMENT_ARRAY_BUFFER;
		default:
			return 0;
		}
	}

	//texture
	static constexpr uint32_t MAX_TEXTURE_COUNT = 8;
	static constexpr GLuint MAX_TEXTURE = GL_TEXTURE0 + 7;

	enum class TextureType
	{
		Texture2D,
		TextureCubeMap
	};

	static GLuint toGL(const TextureType& value) noexcept
	{
		switch (value)
		{
		case TextureType::Texture2D:
			return GL_TEXTURE_2D;
		case TextureType::TextureCubeMap:
			return GL_TEXTURE_CUBE_MAP;
		default:
			return GL_NONE;
		}
	}

	enum class TextureFormat
	{
		RGB,
		RGBA,
		DepthFormat,
		DepthStencilFormat
	};


	static GLuint toGL(const TextureFormat& format) noexcept
	{
		switch (format)
		{
		case TextureFormat::RGB:
			return GL_RGB;
		case TextureFormat::RGBA:
			return GL_RGBA;
		case TextureFormat::DepthFormat:
			return GL_DEPTH_COMPONENT32F;
		case TextureFormat::DepthStencilFormat:
			return GL_DEPTH24_STENCIL8;
		default:
			return 0;
		}
	}

	static uint32_t toStbImageFormat(const TextureFormat& format)
	{
		switch (format)
		{
		case TextureFormat::RGB:
			return STBI_rgb;
		case TextureFormat::RGBA:
			return STBI_rgb_alpha;
		default:
			return 0;
		}
	}

	static uint32_t toPixelSize(const TextureFormat& format)
	{
		switch (format) {
		case TextureFormat::RGB:
			return 24;
		case TextureFormat::RGBA:
			return 32;
		default:
			return 0;
		}
	}

	static uint32_t toByteSize(const TextureFormat& format) 
	{
		switch (format) {
		case TextureFormat::RGB:
			return 3;
		case TextureFormat::RGBA:
			return 4;
		default:
			return 0;
		}
	}

	enum class TextureWrapping
	{
		RepeatWrapping,
		ClampToEdgeWrapping,
		ClampToBorder,
		MirroredRepeatWrapping
	};

	static uint32_t toGL(const TextureWrapping& value) {
		switch (value) {
		case TextureWrapping::RepeatWrapping:
			return GL_REPEAT;
		case TextureWrapping::ClampToEdgeWrapping:
			return GL_CLAMP_TO_EDGE;
		case TextureWrapping::MirroredRepeatWrapping:
			return GL_MIRRORED_REPEAT;
		case TextureWrapping::ClampToBorder:
			return GL_CLAMP_TO_BORDER;
		default:
			return 0;
		}
	}


	enum class TextureFilter
	{
		LinearFilter,
		NearestFilter,
	};

	static uint32_t toGL(const TextureFilter& value) 
	{
		switch (value) {
		case TextureFilter::LinearFilter:
			return GL_LINEAR;
		case TextureFilter::NearestFilter:
			return GL_NEAREST;
		default:
			return 0;
		}
	}

	enum class TextureUsage 
	{
		SamplerTexture,
		RenderTargetTexture   //离屏渲染
	};

	enum  class Side
	{
		FrontSide,
		BackSide,
		DoubleSide,
		None
	};

	static uint32_t toGL(const Side& side) 
	{
		switch (side) {
		case Side::FrontSide:
			return GL_BACK;
		case Side::BackSide:
			return GL_FRONT;
		default:
			return GL_BACK;
		}
	}

	enum class FrontFace 
	{
		FrontClockWise,
		FrontCounterClockWise,
		None
	};

	static uint32_t toGL(const FrontFace& value) 
	{
		switch (value) 
		{
		case FrontFace::FrontCounterClockWise:
			return GL_CCW;
		case FrontFace::FrontClockWise:
			return GL_CW;
		default:
			return GL_NONE;
		}
	}

	enum class BlendingType 
	{
		DefaultBlending,
		CustomBlending,
		NoBlending,
		None
	};
	enum class BlendingFactor 
	{
		Zero,
		One,
		SrcAlpha,
		OneMinusSrcAlpha,
		None
	};

	static uint32_t toGL(const BlendingFactor& value) 
	{
		switch (value) 
		{
		case BlendingFactor::SrcAlpha:
			return GL_SRC_ALPHA;
		case BlendingFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case BlendingFactor::One:
			return GL_ONE;
		case BlendingFactor::Zero:
			return GL_ZERO;
		default:
			return GL_NONE;
		}
	}

	enum class BlendingEquation 
	{
		AddEquation,
		MinEquation,
		MaxEquation,
		None
	};

	static uint32_t toGL(const BlendingEquation& value) 
	{
		switch (value) 
		{
		case BlendingEquation::AddEquation:
			return GL_FUNC_ADD;
		case BlendingEquation::MaxEquation:
			return GL_MAX;
		case BlendingEquation::MinEquation:
			return GL_MIN;
		default:
			return GL_NONE;
		}
	}

	enum class CompareFunction 
	{
		Less,
		LessOrEqual,
		Bigger,
		BiggerOrEqual,
		None
	};

	static uint32_t toGL(const CompareFunction& value) 
	{
		switch (value) 
		{
		case CompareFunction::Less:
			return GL_LESS;
		case CompareFunction::Bigger:
			return GL_GREATER;
		case CompareFunction::LessOrEqual:
			return GL_LEQUAL;
		case CompareFunction::BiggerOrEqual:
			return GL_GEQUAL;
		default:
			return GL_NONE;
		}
	}

	enum class DrawMode 
	{
		Lines,
		LinesStrip,
		Triangles
	};

	static uint32_t toGL(const DrawMode& value) 
	{
		switch (value) 
		{
		case DrawMode::Lines:
			return GL_LINES;
		case DrawMode::LinesStrip:
			return GL_LINE_STRIP;
		case DrawMode::Triangles:
			return GL_TRIANGLES;
		default:
			return GL_NONE;
		}
	}


	//attributes
	static const std::unordered_map<std::string, uint32_t> LOCATION_MAP =
	{
		{"position", 0},
		{"normal", 1},
		{"color", 2},
		{"uv", 3},
		{"skinIndex", 4},
		{"skinWeight", 5},
		{"tangent", 6},
		{"bitangent", 7}
	};

}


