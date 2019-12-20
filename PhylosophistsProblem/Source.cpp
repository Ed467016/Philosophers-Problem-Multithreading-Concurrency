#include <iostream>
#include <thread>
#include <Windows.h>

#define __LEFT 0
#define __RIGHT 1
#define __COUNT 5
#define __STD std::
#define __SEED 10000
#define __FORKS_COUNT 2

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

struct phylosopher{
	HANDLE* fHandles;
	int id;
public:
	phylosopher(
		HANDLE& l,
		HANDLE& r, 
		int _id)
	{
		fHandles = new HANDLE[__FORKS_COUNT];
		fHandles[__LEFT] = l;
		fHandles[__RIGHT] = r;

		id = _id;
	}

	static DWORD WINAPI eat(LPVOID that) {
		while (true)
		{
			auto _this = (phylosopher*)that;
			auto sleepTime = (DWORD)(rand() % __SEED);
			auto thinkTime = (DWORD)(rand() % __SEED);

			WaitForMultipleObjects(TWOSTOPBITS, _this->fHandles, TRUE, INFINITE);
			WaitForSingleObject(hMutex, INFINITE);
			_STD cout << _this->id << " STARTED EATING\n";
			ReleaseMutex(hMutex);

			Sleep(sleepTime);

			WaitForSingleObject(hMutex, INFINITE);
			_STD cout << _this->id << " FINISHED EATING\n";
			ReleaseMutex(hMutex);

			ReleaseMutex(_this->fHandles[__LEFT]);
			ReleaseMutex(_this->fHandles[__RIGHT]);

			Sleep(thinkTime);
		};

		return (DWORD)1;
	}
};

int main() {
	DWORD tDW;
	srand((unsigned)time(NULL));
	HANDLE tHandles[__COUNT];
	HANDLE mHandles[__COUNT];
	phylosopher* phylosophers[__COUNT];

	for (size_t i = 0; i < __COUNT; i++)
	{
		auto mHandle = CreateMutex(NULL, FALSE, NULL);
		mHandles[i] = mHandle;
	};
	int i = 0;
	for (; i < __COUNT; i++)
	{
		phylosopher* ph = new phylosopher(mHandles[i], mHandles[(i + 1) % __COUNT], i);

		tHandles[i] = CreateThread(
			NULL,
			1024 * sizeof(DWORD),
			(LPTHREAD_START_ROUTINE) phylosopher::eat,
			ph,
			0,
			&tDW
		);

		phylosophers[i] = ph;
	};

	WaitForMultipleObjects(__COUNT, tHandles, TRUE, 25000);

	for (size_t i = 0; i < __COUNT; i++)
	{
		TerminateThread(tHandles[i], 0);
		CloseHandle(tHandles[i]);
		CloseHandle(mHandles[i]);
	}

	__STD cout << "DINNER FINISHED";
	getchar();
}