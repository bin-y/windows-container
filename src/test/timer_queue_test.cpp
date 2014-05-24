#include <cstdio>
#include <memory>
#include "../timer_queue.hpp"

using namespace std;
using namespace winc;

int remain = 5;

void queue(const shared_ptr<timer_queue> &q)
{
	if (--remain >= 0) {
		q->queue([q](){
			printf("timer elapsed\n");
			queue(q);
		}, 1000);
	}
}

void test()
{
	auto q = make_shared<timer_queue>();
	queue(q);
	q->queue([](){
		printf("timer 3 elapsed\n");
	}, 3500);
	q->queue([](){
		printf("timer 2 elapsed\n");
	}, 2500);
	q->queue([](){
		printf("timer 2a elapsed\n");
	}, 2500);
}

int main()
{
	test();
	ExitThread(0);
}