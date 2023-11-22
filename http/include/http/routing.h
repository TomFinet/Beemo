#pragma once

#include <http/msg.h>

/* The purpose of this file is to route a request to the correct resource handler.
This server does not forward requests onwards, so if a request is not destined to it,
we return an error. */

namespace http
{
    /* the first step to routing is to ensure the message is semantically correct. */ 
    void validate(req *const req, const struct config &config);

}