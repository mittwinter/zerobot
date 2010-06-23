namespace foo {

class C {
};

class A : public C {
	public:
		A(int _foo, bool _bar) : foo(_foo), bar(_bar) {}

	protected:
		int foo;
		bool bar;
};

class B : public C {
	public:
		B(bool _bar) : bar(_bar) {}

		operator A() const {
			return A(0, bar);
		}

	protected:
		bool bar;
};

}

int main() {
	foo::A a(1, false);
	a = foo::B(true);
}


