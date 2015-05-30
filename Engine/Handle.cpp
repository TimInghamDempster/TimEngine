namespace Engine
{
	template<class Tag, class impl, impl default_value>
	class Handle
	{
	public:
		static Handle Invalid() { return Handle(); }

		// Defaults to ID::invalid()
		Handle() : m_val(default_value) { }

		// Explicit constructor:
		explicit Handle(impl val) : m_val(val) { }

		// TODO: this causes an "invalid storage class" compile error for some reason.
		// Explicit conversion to get back the impl:
		//explicit operator impl() const { return m_val; }

		friend bool operator==(Handle a, Handle b) { return a.m_val == b.m_val; }
		friend bool operator!=(Handle a, Handle b) { return a.m_val != b.m_val; }

		inline impl GetValue() { return m_val; }

	private:
		impl m_val;
	};
	

	template<class HandleType, class DataContainer>
	class StableHandleList
	{
		std::vector<char> m_alive;
		std::vector<HandleType> m_freeHandles;

		bool m_canGrow;
		
	public:

		StableHandleList(int initialCount, bool canGrow) : m_canGrow(canGrow)
		{
			m_alive.reserve(initialCount);
		}

		DataContainer lists;

		HandleType AddItem()
		{
			HandleType handle;
			if(m_freeHandles.size() != 0)
			{
				handle = *m_freeHandles.end();
				m_freeHandles.pop_back();
				m_alive[handle.GetValue()] = true;
			} 
			else if (m_canGrow)
			{
				handle = HandleType(m_alive.size());
				m_alive.push_back(true);
			}
			return handle;
		};

		bool RemoveItem(HandleType handle)
		{
			if(handle != HandleType::Invalid())
			{
				int32 index = handle.GetValue();
				if(index < m_alive.size())
				{
					m_alive[index] = false;
					m_freeHandles.push_back(handle);
					return true;
				}
			}
			return false;
		}
	};
}