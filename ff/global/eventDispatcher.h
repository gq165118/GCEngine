#pragma once
#include "base.h"

namespace ff
{
	//1 可以注册某一个对象的某一个函数
	//2 可以取消对某个事件的监听函数

	//收发的消息体
	class EventBase
	{
	public:
		using Ptr = std::shared_ptr<EventBase>;
		static Ptr create(const std::string& eventName)
		{
			return std::make_shared<EventBase>(eventName);
		}

		EventBase(const std::string& eventName) noexcept;

		~EventBase() noexcept;

		std::string m_eventName;
		void* m_target{ nullptr };   //消息主体，哪个对象发出的，存储其指针
		void* m_pUserData{ nullptr };	//根据不同的需求，将userData设置成不同的对象指针

	};

	class EventDispatcher
	{
	public:
		template<typename T>
		using TypedFunctionPointer = void(T::*)(const EventBase::Ptr&);  //类内函数指针

		using EventFunction = std::function<void(const EventBase::Ptr&)>;

		struct Listener
		{
			using Ptr = std::shared_ptr<Listener>;
			static Ptr create() { return std::make_shared<Listener>(); }

			Listener() noexcept {}
			~Listener() noexcept
			{
				if (m_functionPointerDescriptor.m_addr)
				{
					free(m_functionPointerDescriptor.m_addr);
				}
			}

			struct FunctionPointerDescriptor
			{
				void* m_addr = nullptr;   //函数指针地址
				size_t m_size{ 0 };

				bool operator==(const FunctionPointerDescriptor& other) const
				{
					if (other.m_size != m_size)
					{
						return false;
					}

					if (memcmp(m_addr, other.m_addr, m_size) != 0)
					{
						return false;
					}

					return true;
				}
			};

			template<typename T>
			static FunctionPointerDescriptor buildFunctionPointer(TypedFunctionPointer<T> functionPointer) noexcept
			{
				//计算本指针有多大，不是指针指向的内容
				size_t s = sizeof(TypedFunctionPointer<T>);
				void* p = malloc(s);

				//placement New 先分配一块内存，然后通过placementNew调用其构造函数/默认构造
				new(p)TypedFunctionPointer<T>(functionPointer);

				Listener::FunctionPointerDescriptor fpd;
				fpd.m_addr = p;
				fpd.m_size = s;

				return fpd;
			}
			
			//caller 
			void* m_target = nullptr;

			FunctionPointerDescriptor m_functionPointerDescriptor{};

			//函数指针管理
			std::function<void(const EventBase::Ptr&)> m_function = nullptr;
		};

		static bool listenerIsEqual(const Listener::Ptr& l0, const Listener::Ptr& l1)
		{
			//调用对象的地址一致，且对应的函数地址一致
			return l0->m_target == l1->m_target && l0->m_functionPointerDescriptor == l1->m_functionPointerDescriptor;
		}

		//存储监听事件名称_监听函数队列，同一个事件名称，可能会由多个Listener(多个对象函数)
		using ListenerQueue = std::vector<Listener::Ptr>;

		EventDispatcher() noexcept;
		~EventDispatcher() noexcept;

		static EventDispatcher* getInstance();

		template<typename T>
		void addEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept;

		template<typename T>
		void removeEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept;

		void dispatchEvent(const EventBase::Ptr& event);

	protected:
		std::unordered_map<std::string, ListenerQueue> m_listeners;
		static EventDispatcher* m_instance;

	};

	template<typename T>
	void EventDispatcher::addEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept
	{
		//1 通过事件名找到对应的监听队列，没找到则插入一个新的队列
		auto queueIter = m_listeners.find(name);

		if (queueIter == m_listeners.end())
		{
			//queueIter = (m_listeners.insert(std::make_shared(name, ListenerQueue()))).first;
			//inserted 为true表示新插入的，否则不是
			auto [queueIter, inserted] = m_listeners.insert({name, ListenerQueue()});
		}

		auto& ListenerQueue = queueIter->second;

		//2 开始构建listener
		Listener::Ptr listener = Listener::create();
		listener->m_target = target;

		//通过bind生成的function无法进行对比，无法使用function1 == function2来判断是否相等
		//传入的functionPointer是bind之前的结果，即单独的函数指针地址
		listener->m_functionPointerDescriptor = Listener::buildFunctionPointer(functionPointer);
		
		//将类内函数指针与对象指针结合在一起
		auto function = std::bind(functionPointer, target, std::placeholders::_1);
		listener->m_function = function;
		
		//3 查看监听队列中是否已经存在listener，不存在则加入到队列中
		auto listenerIter = std::find_if(ListenerQueue.begin(), ListenerQueue.end(),
			[&](const Listener::Ptr& l)
			{
				return EventDispatcher::listenerIsEqual(listener, l);
			});

		if (listenerIter != ListenerQueue.end())
		{
			return;
		}

		ListenerQueue.push_back(listener);
	}

	template<typename T>
	void EventDispatcher::removeEventListener(const std::string& name, T* target, TypedFunctionPointer<T> functionPointer) noexcept
	{
		auto queueIter = m_listeners.find(name);

		if (queueIter == m_listeners.end())
		{
			return;
		}

		auto& listenerQueue = queueIter->second;

		auto function = std::bind(functionPointer, target, std::placeholders::_1);
		Listener::Ptr listener = Listener::create();
		listener->mTarget = target;
		listener->mFuncionPointerDescriptor = Listener::buildFunctionPointer(functionPointer);
		listener->mFunction = function;

		auto listenerIter = std::find_if(ListenerQueue.begin(), ListenerQueue.end(),
			[&](const Listener::Ptr& l)
			{
				return EventDispatcher::listenerIsEqual(listener, l);
			});

		if (listenerIter != ListenerQueue.end())
		{
			listenerIter.erase(listenerIter);
		}
	}

}