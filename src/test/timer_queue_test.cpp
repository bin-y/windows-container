#include <cstdio>
#include <memory>
#include "../timer_queue.hpp"

using namespace std;
using namespace winc;

timer_queue q;
int remain = 5;

void queue()
{
	if (--remain >= 0) {
		q.queue([](){
			printf("timer elapsed\n");
			queue();
		}, 1000);
	}
}

int main()
{
	queue();
	q.queue([](){
		printf("timer 2 elapsed\n");
	}, 2500);
}