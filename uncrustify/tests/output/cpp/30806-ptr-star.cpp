int dx = m_ClipBox.GetWidth() * GetZoom();

m_ClipBox.m_Pos.y = PaintClipBox.y * GetZoom();

int *i;
char *i;

int MyFunc(std::string &s, char *) {
	char *c = const_cast<char *>(s.c_str());
}

int YerFunc(std::string &s, char **) {
	char **c;
	int a = b[0] * c;
}

int *X(int *i, int *);

int *i = &a;
int *i = *b;
int *i = &*c;

int *Aclass::X(int *i, int *);

class Aclass {
int *X(int *i, int *);
}
extern "C" {
int foo1(int *a);
int foo2(sometype *a);
}
int bar1(int *a);
int bar2(sometype *a);

struct X
{
	int *a; // 3:5

	int f()
	{
		return *b; // 7:8
	}
	int g()
	{
		return *c; // 11:8
	}
};
