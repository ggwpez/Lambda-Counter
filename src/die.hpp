#pragma once

#include <cstdio>
#include <cstdlib>

#define die(err) do { fprintf(stderr, "Error: '%s'", (err)); abort(); } while (0)
