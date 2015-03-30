namespace Utils
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

		impl GetValue() { return m_val; }

	private:
		impl m_val;
	};
}