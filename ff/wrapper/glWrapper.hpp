/**
 * @namespace ff::gl
 * @brief 轻量级 OpenGL 状态封装与错误检查工具集。
 *
 * 概述：
 * - 提供对常用 GL 状态接口的薄封装，并在每次调用后执行错误检查（checkError）。
 * - 通过循环读取 glGetError() 清空错误队列，打印错误字符串并在调试期断言。
 * - 适合在渲染管线配置阶段（混合、深度、清屏、Uniform 设置等）统一使用，提升可读性。
 *
 * 主要功能：
 * - 状态切换：enable()/disable()
 * - 颜色/清屏：clearColor()
 * - Uniform：uniform1i()/uniform1iv()
 * - 混合：blendFuncSeparate()/blendEquationSeparate()
 * - 深度：depthMask()/depthFunc()/clearDepth()
 *
 * 使用示例：
 * @code
 * // 启用混合并配置方程/因子
 * ff::gl::enable(GL_BLEND);
 * ff::gl::blendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
 *                           GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
 * ff::gl::blendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
 *
 * // 配置深度
 * ff::gl::enable(GL_DEPTH_TEST);
 * ff::gl::depthMask(GL_TRUE);
 * ff::gl::depthFunc(GL_LEQUAL);
 * ff::gl::clearDepth(1.0);   // 清深度值，需配合 glClear(GL_DEPTH_BUFFER_BIT)
 *
 * // 设置清屏颜色
 * ff::gl::clearColor(0.f, 0.f, 0.f, 1.f);
 *
 * // 设置 Uniform
 * ff::gl::uniform1i(location, 0);
 * @endcode
 *
 * 设计要点与注意：
 * - 错误检查：checkError() 会读取并打印所有错误码（如 INVALID_ENUM/OPERATION 等），
 *   然后在 needsAssert 为真时触发断言。建议仅在 Debug 构建中启用严格断言。
 * - 性能建议：频繁调用 glGetError 具有开销；Release 构建可通过宏屏蔽断言或条件编译。
 * - 上下文要求：所有接口需在持有有效 GL 上下文的渲染线程中调用。
 * - clearDepth：仅设置清深度的目标值，不会立即清除深度缓冲；需随后调用 glClear。
 * - uniform1iv：参数 value 为指向连续 GLint 缓冲的指针，size 表示元素个数。
 *
 * 可扩展性：
 * - 可按需继续添加对光栅化（面剔除、正面朝向）、模板测试、视口/剪裁等状态的封装，
 *   并复用 checkError() 保持一致的错误处理策略。
 *
 * @author qiang.guo
 * @date 2025-08-29
 */

#pragma once
#include "../global/base.h"

namespace ff {
	namespace gl {
		static void checkError() {
			GLenum errorCode;
			bool needsAssert = false;
			std::string error;
			while ((errorCode = glGetError()) != GL_NO_ERROR)
			{
				needsAssert = true;
				switch (errorCode)
				{
				case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
				case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
				case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
				case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
				case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
				case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
				}
				std::cout << error << std::endl;
			}
			assert(!needsAssert);
		}

		//state
		static void enable(GLenum cap) {
			glEnable(cap);
			checkError();
		}

		static void disable(GLenum cap) {
			glDisable(cap);
			checkError();
		}

		static void clearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			glClearColor(r, g, b, a);
			checkError();
		}

		static void uniform1iv(GLint location, GLsizei size, GLint* value) {
			glUniform1iv(location, size, value);
			checkError();
		}

		static void uniform1i(GLint location, GLint value) {
			glUniform1i(location, value);
			checkError();
		}


		//blend
		static void blendFuncSeparate(
			GLenum srcRGB,
			GLenum dstRGB,
			GLenum srcAlpha,
			GLenum dstAlpha) {

			glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
			checkError();
		}

		static void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
			glBlendEquationSeparate(modeRGB, modeAlpha);
			checkError();
		}

		//depth
		static void depthMask(GLboolean flag) {
			glDepthMask(flag);
			checkError();
		}

		static void depthFunc(GLenum func) {
			glDepthFunc(func);
			checkError();
		}

		static void clearDepth(GLdouble depth) {
			glClearDepth(depth);
			checkError();
		}
	}
	
}
