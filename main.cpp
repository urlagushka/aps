#include "src/pqueue.hpp"
#include "src/request/request.hpp"

bool pred(request_t)
{
    return true;
}

int main()
{
    request_t dnull;
    pqueue_t< request_t, pred > queue(2, dnull);
}
