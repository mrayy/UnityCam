

#ifndef __FILTERREGISTER__
#define __FILTERREGISTER__


class FilterRegister
{
protected:

	int _refCount;
	bool _registered;
	static FilterRegister* _instance;

	void _Register();
	void _UnRegister();
public:

	FilterRegister();
	virtual ~FilterRegister();

	void AddRef();
	void SubRef();


	static FilterRegister* Instance();
};



#endif
