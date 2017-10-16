#include <stdio.h>
#include "work.h"
#include "event.h"

struct samp_request {
    int val1;
    int val2;
	struct work work;
};

void work_fn(struct work *work) {
    struct samp_request *req = container_of(work, struct samp_request, work);
    printf("val1: %d, val2: %d. \n", req->val1, req->val2);
    free(req);
}
int main(){
	if (init_event(4096) < 0) {
		return -1;
	}
    
	if (init_work_queue()) {
		return -1;
    }

    struct work_queue* fixwork = create_fixed_work_queue("WayFixed", 4);


    struct samp_request *req = malloc(sizeof(struct samp_request));
    req->val1 = 1;
    req->val2 = 2;
    req->work.fn = work_fn;
    req->work.done = NULL;
    
    queue_work(fixwork, &req->work);
    while(1) {
        sleep(1);
    }
    return 0;
}
